#include <getopt.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "analyze.h"
#include "diff.h"
#include "explain.h"
#include "history.h"
#include "export.h"
#include "import.h"
#include "fmt.h"
#include "report.h"
#include "scan.h"
#include "tree.h"
#include "tui.h"

static void usage(FILE *f)
{
	fprintf(f,
		"usage: diskmoon [options] [path]\n"
		"       diskmoon [options] -f FILE\n"
		"\n"
		"Scan a directory tree and show where the space went.\n"
		"In a terminal it opens an interactive view (press ? there for keys); otherwise,\n"
		"or with -o or -F, it writes the complete report. The path defaults to /.\n"
		"\n"
		"  -o, --output FILE     write to FILE ('-' for stdout); .json and .csv pick the format\n"
		"  -F, --format FMT      txt (report), ncdu (ncdu JSON export) or csv\n"
		"  -f, --from FILE       read an ncdu JSON export instead of scanning ('-' for stdin)\n"
		"  -s, --save[=FILE]     also keep this scan as ncdu JSON (default: the scan history)\n"
		"  -c, --compare FILE    show what changed since an older scan; 'last' = newest in the history\n"
		"  -d, --depth N         show the tree down to depth N (sizes stay complete)\n"
		"  -m, --min-size SIZE   fold entries smaller than SIZE (e.g. 10M, 1G)\n"
		"  -n, --top N           list the N largest files (default %d)\n"
		"  -j, --jobs N          scanner threads (default: CPUs up to %d, %d on spinning disks)\n"
		"  -x, --one-file-system stay on the filesystem of the starting path\n"
		"  -a, --apparent-size   count file length instead of allocated blocks\n"
		"      --duplicates[=SIZE] also find files with the same content (default from 1M)\n"
		"      --all-fs          also enter tmpfs, proc, fuse and other non-disk mounts\n"
		"      --dirs-only       hide files in the tree (they still count in totals)\n"
		"      --ascii           draw the tree with ASCII characters\n"
		"  -q, --quiet           no progress on stderr\n"
		"  -h, --help            show this help\n"
		"  -V, --version         show the version\n",
		DEFAULT_TOP_FILES, MAX_AUTO_JOBS, ROTATIONAL_JOBS);
}

static int fail(const char *what, const char *value)
{
	fprintf(stderr, "diskmoon: invalid %s: %s\n", what, value);
	return -1;
}

static int parse_int(const char *s, int *out)
{
	char *end;
	long v = strtol(s, &end, 10);
	if (end == s || *end || v < 0 || v > INT_MAX)
		return -1;
	*out = (int)v;
	return 0;
}

enum { OPT_ALL_FS = 256, OPT_DIRS_ONLY, OPT_ASCII, OPT_DUPLICATES };

static const struct option long_options[] = {
	{ "output", required_argument, NULL, 'o' },
	{ "format", required_argument, NULL, 'F' },
	{ "from", required_argument, NULL, 'f' },
	{ "save", optional_argument, NULL, 's' },
	{ "compare", required_argument, NULL, 'c' },
	{ "depth", required_argument, NULL, 'd' },
	{ "min-size", required_argument, NULL, 'm' },
	{ "top", required_argument, NULL, 'n' },
	{ "jobs", required_argument, NULL, 'j' },
	{ "one-file-system", no_argument, NULL, 'x' },
	{ "apparent-size", no_argument, NULL, 'a' },
	{ "all-fs", no_argument, NULL, OPT_ALL_FS },
	{ "dirs-only", no_argument, NULL, OPT_DIRS_ONLY },
	{ "ascii", no_argument, NULL, OPT_ASCII },
	{ "duplicates", optional_argument, NULL, OPT_DUPLICATES },
	{ "quiet", no_argument, NULL, 'q' },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'V' },
	{ 0 },
};

static int parse_format(const char *s, enum format *out)
{
	static const char *const names[] = { [FORMAT_TXT] = "txt", [FORMAT_NCDU] = "ncdu", [FORMAT_CSV] = "csv" };
	for (int i = FORMAT_TXT; i <= FORMAT_CSV; i++)
		if (!strcmp(s, names[i])) {
			*out = (enum format)i;
			return 0;
		}
	if (strcmp(s, "json"))
		return -1;
	*out = FORMAT_NCDU;
	return 0;
}

static enum format format_from_name(const char *name)
{
	const char *dot = name ? strrchr(name, '.') : NULL;
	if (dot && !strcmp(dot, ".json"))
		return FORMAT_NCDU;
	return dot && !strcmp(dot, ".csv") ? FORMAT_CSV : FORMAT_TXT;
}

static int apply_option(struct options *o, int opt)
{
	switch (opt) {
	case 'o': o->output = optarg; return 0;
	case 'F': return parse_format(optarg, &o->format) ? fail("format", optarg) : 0;
	case 'f': o->from = optarg; return 0;
	case 's': o->save = optarg ? optarg : ""; return 0;
	case 'c': o->compare = optarg; return 0;
	case 'd': return parse_int(optarg, &o->max_depth) ? fail("depth", optarg) : 0;
	case 'm': return parse_size(optarg, &o->min_size) ? 0 : fail("size", optarg);
	case 'n': return parse_int(optarg, &o->top) ? fail("count", optarg) : 0;
	case 'j': return parse_int(optarg, &o->jobs) ? fail("job count", optarg) : 0;
	case 'x': o->one_fs = true; return 0;
	case 'a': o->apparent = true; return 0;
	case 'q': o->quiet = true; return 0;
	case OPT_ALL_FS: o->all_fs = true; return 0;
	case OPT_DIRS_ONLY: o->no_files = true; return 0;
	case OPT_ASCII: o->ascii = true; return 0;
	case OPT_DUPLICATES:
		o->dupes_min = DUPES_MIN;
		return optarg && !parse_size(optarg, &o->dupes_min) ? fail("size", optarg) : 0;
	case 'h': usage(stdout); exit(0);
	case 'V': puts("diskmoon " DISKMOON_VERSION); exit(0);
	default: usage(stderr); return -1;
	}
}

static int parse_args(int argc, char **argv, struct options *o)
{
	*o = (struct options){ .max_depth = -1, .top = DEFAULT_TOP_FILES };
	int opt;
	while ((opt = getopt_long(argc, argv, "o:F:f:s::c:d:m:n:j:xaqhV", long_options, NULL)) != -1)
		if (apply_option(o, opt))
			return -1;
	if (argc - optind > 1)
		return fail("arguments", "only one path is accepted");
	o->interactive = !o->output && o->format == FORMAT_AUTO && !o->compare && isatty(STDIN_FILENO) &&
			 isatty(STDOUT_FILENO);
	if (o->format == FORMAT_AUTO)
		o->format = format_from_name(o->output);
	if (o->compare && o->format == FORMAT_NCDU)
		return fail("format", "a comparison is written as txt or csv");
	if (o->from)
		return optind < argc ? fail("arguments", "a path cannot be combined with --from") : 0;
	const char *path = optind < argc ? argv[optind] : "/";
	o->root = realpath(path, NULL);
	if (!o->root) {
		perror(path);
		return -1;
	}
	return 0;
}

static void collect_explain(const struct scan_result *r, const struct tree_stats *s, struct explain *e)
{
	int *mounts = malloc((s->mounts.len + 1) * sizeof *mounts);
	if (!mounts)
		return;
	size_t n = 0;
	mounts[n++] = node_mount(r->root);
	for (size_t i = 0; i < s->mounts.len; i++)
		mounts[n++] = node_mount(s->mounts.items[i]);
	explain_collect(e, mounts, n);
	free(mounts);
}

static FILE *open_output(const struct options *o, char *name, size_t cap)
{
	if (o->output && strcmp(o->output, "-"))
		snprintf(name, cap, "%s", o->output);
	else if (!o->output && isatty(STDOUT_FILENO)) {
		time_t now = time(NULL);
		struct tm tm;
		localtime_r(&now, &tm);
		static const char *const ext[] = { [FORMAT_TXT] = "txt", [FORMAT_NCDU] = "json", [FORMAT_CSV] = "csv" };
		char stamp[64];
		strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tm);
		snprintf(name, cap, DEFAULT_REPORT_PREFIX "-%s.%s", stamp, ext[o->format]);
	} else {
		name[0] = '\0';
		return stdout;
	}
	FILE *f = fopen(name, "w");
	if (!f)
		perror(name);
	return f;
}

static int load_tree(const struct options *o, const char *file, struct scan_result *r, struct tree_stats *s)
{
	if (file ? import_ncdu(o, file, r) : scan_run(o, r))
		return -1;
	tree_finalize(r->root, (size_t)o->top, s);
	return 0;
}

static int load_previous(const struct options *o, const struct scan_result *now, struct scan_result *old)
{
	static char path[PATH_MAX];
	const char *file = o->compare;
	if (!strcmp(file, "last")) {
		if (!history_latest(now->root->name, NULL, path, sizeof path)) {
			fprintf(stderr, "diskmoon: no saved scan of %s yet; run once with --save first\n", now->root->name);
			return -1;
		}
		file = path;
	}
	struct tree_stats s;
	int failed = load_tree(o, file, old, &s);
	tree_stats_free(&s);
	return failed;
}

static void save_scan(const struct options *o, const struct scan_result *r)
{
	char path[PATH_MAX];
	snprintf(path, sizeof path, "%s", o->save);
	if (!history_save(o, r, path, sizeof path))
		perror(path[0] ? path : "diskmoon: scan history folder");
	else if (!o->quiet)
		fprintf(stderr, "scan saved to %s\n", path);
}

static void write_output(FILE *f, const struct options *o, const struct scan_result *r, const struct tree_stats *s,
			 const struct explain *e, const struct analysis *a, const struct scan_result *old)
{
	if (old && o->format == FORMAT_CSV)
		diff_write_csv(f, o, old, r);
	else if (old)
		diff_write(f, o, old, r);
	else if (o->format == FORMAT_NCDU)
		export_ncdu(f, o, r);
	else if (o->format == FORMAT_CSV)
		export_csv(f, o, r);
	else
		report_write(f, o, r, s, e, a);
}

static int deliver(const struct options *o, struct scan_result *r, struct tree_stats *s, struct explain *e,
		   struct analysis *a, struct scan_result *old)
{
	if (o->interactive)
		return tui_run(o, r, s, e, a);
	char name[PATH_MAX];
	FILE *f = open_output(o, name, sizeof name);
	if (!f)
		return 1;
	setvbuf(f, NULL, _IOFBF, OUTPUT_BUFFER);
	write_output(f, o, r, s, e, a, old);
	if (fclose(f)) {
		perror(name[0] ? name : "stdout");
		return 1;
	}
	if (name[0] && !o->quiet)
		fprintf(stderr, "written to %s\n", name);
	return 0;
}

static int run(struct options *o, struct scan_result *r, struct tree_stats *s)
{
	struct scan_result old = { 0 };
	if (o->compare && load_previous(o, r, &old))
		return 1;
	if (o->save)
		save_scan(o, r);
	struct explain e = { 0 };
	struct analysis a = { 0 };
	if (!o->from && !o->compare && o->format == FORMAT_TXT)
		collect_explain(r, s, &e);
	if (!o->compare && o->format == FORMAT_TXT)
		analyze_run(o, r, &a);
	int status = deliver(o, r, s, &e, &a, o->compare ? &old : NULL);
	analyze_free(&a);
	explain_free(&e);
	scan_result_free(&old);
	return status;
}

int main(int argc, char **argv)
{
	struct options o;
	mallopt(M_MMAP_THRESHOLD, MMAP_THRESHOLD);
	if (parse_args(argc, argv, &o))
		return 1;
	struct scan_result r = { 0 };
	struct tree_stats s = { 0 };
	int status = load_tree(&o, o.from, &r, &s) ? 1 : run(&o, &r, &s);
	tree_stats_free(&s);
	scan_result_free(&r);
	free(o.root);
	return status;
}

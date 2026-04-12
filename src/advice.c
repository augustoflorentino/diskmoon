#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include "advice.h"
#include "config.h"
#include "fmt.h"
#include "mounts.h"

#define TYPE_ROWS 15
#define HOLDERS_SHOWN 3

struct holder {
	int pid;
	const char *comm;
	uint64_t size;
};

static const char *scan_root;

static bool held_here(const struct held_file *h)
{
	size_t len = strlen(scan_root);
	return !strcmp(scan_root, "/") || (!strncmp(h->path, scan_root, len) && (!h->path[len] || h->path[len] == '/'));
}

static uint64_t held_total(const struct explain *e)
{
	uint64_t total = 0;
	for (size_t i = 0; i < e->nheld; i++)
		total += held_here(&e->held[i]) ? e->held[i].size : 0;
	return total;
}

static uint64_t hidden_total(const struct explain *e)
{
	uint64_t total = 0;
	for (size_t i = 0; i < e->nhidden; i++)
		if (!e->hidden[i].err)
			total += e->hidden[i].size;
	return total;
}

static void summary_disk(FILE *f, const struct scan_result *r)
{
	const struct mount *m = mounts_get(node_mount(r->root));
	struct statvfs v;
	if (r->source || !m || statvfs(r->root->name, &v))
		return;
	uint64_t used = (uint64_t)(v.f_blocks - v.f_bfree) * v.f_frsize, avail = (uint64_t)v.f_bavail * v.f_frsize;
	char pct[FMT_LEN], free[FMT_LEN];
	fmt_percent(used, used + avail, pct);
	fmt_size(avail, free);
	fprintf(f, "  The disk it lives on (%s, mounted at ", m->source);
	fmt_write_path(f, m->point);
	fprintf(f, ") is %s full, with %s free.\n", pct, free);
}

static void summary_hot(FILE *f, const struct scan_result *r, const struct analysis *a)
{
	const struct node *hot = a->nhot ? a->hot[a->nhot - 1] : node_child(r->root);
	char path[LONG_PATH], size[FMT_LEN], pct[FMT_LEN];
	if (!hot || !node_size(r->root))
		return;
	node_path(hot, path, sizeof path);
	fmt_size(node_size(hot), size);
	fmt_percent(node_size(hot), node_size(r->root), pct);
	fputs("  The heaviest spot is ", f);
	fmt_write_path(f, path);
	fprintf(f, " (%s, %s of everything).\n", size, pct);
}

static void summary_free(FILE *f, const struct analysis *a, const struct explain *e)
{
	char safe[FMT_LEN], check[FMT_LEN], held[FMT_LEN];
	fmt_size(a->safe, safe);
	fmt_size(a->check + held_total(e) + hidden_total(e) + a->dupes.wasted, check);
	fmt_size(held_total(e), held);
	if (a->safe || a->check)
		fprintf(f, "  About %s can be freed safely with the commands in WHAT YOU CAN FREE,\n"
			   "  and %s more after a quick check.\n", safe, check);
	else
		fputs("  No known caches or build leftovers worth cleaning were found.\n", f);
	if (held_total(e))
		fprintf(f, "  %s is held by deleted files that programs still keep open (see NOT SEEN).\n", held);
}

void advice_summary(FILE *f, const struct scan_result *r, const struct explain *e, const struct analysis *a)
{
	char size[FMT_LEN], files[FMT_LEN], dirs[FMT_LEN], errors[FMT_LEN];
	fmt_size(node_size(r->root), size);
	fmt_count(r->files, files);
	fmt_count(r->dirs, dirs);
	fmt_count(r->nerrors, errors);
	scan_root = r->root->name;
	fmt_write_section(f, "SUMMARY");
	fputs("  ", f);
	fmt_write_path(f, r->root->name);
	fprintf(f, " holds %s in %s files and %s folders.\n", size, files, dirs);
	summary_disk(f, r);
	summary_hot(f, r, a);
	summary_free(f, a, e);
	if (r->nerrors)
		fprintf(f, "  %s path%s could not be read%s.\n", errors, r->nerrors == 1 ? "" : "s",
			r->source || !geteuid() ? "" : "; run with sudo to include them");
}

static void write_command(FILE *f, const struct finding *fd)
{
	const char *me = getenv("HOME");
	const char *user = !strncmp(fd->home, "/home/", 6) && (!me || strcmp(me, fd->home)) ? fd->home + 6 : NULL;
	fputs("                  run: ", f);
	if (user)
		fprintf(f, "(as %s) ", user);
	fprintf(f, "%s\n", fd->rule->how);
}

static void write_places(FILE *f, const struct finding *fd)
{
	char path[LONG_PATH], size[FMT_LEN], more[FMT_LEN];
	for (size_t i = 0; i < FINDING_PATHS && fd->paths[i]; i++) {
		node_path(fd->paths[i], path, sizeof path);
		fputs(i ? "                       " : "                  at:  ", f);
		fmt_write_path(f, path);
		if (fd->count > 1) {
			fmt_size(node_size(fd->paths[i]), size);
			fprintf(f, "  (%s)", size);
		}
		fputc('\n', f);
	}
	if (fd->count > FINDING_PATHS) {
		fmt_count(fd->count - FINDING_PATHS, more);
		fprintf(f, "                       ... and %s more\n", more);
	}
}

static void write_finding(FILE *f, const struct finding *fd)
{
	char size[FMT_LEN], count[FMT_LEN];
	fmt_size(fd->size, size);
	fmt_count(fd->count, count);
	fprintf(f, "%9s  %-5s  %s", size, fd->rule->safety == SAFETY_SAFE ? "safe" : "check", fd->rule->what);
	if (fd->count > 1)
		fprintf(f, "  (%s folders)", count);
	fputc('\n', f);
	write_command(f, fd);
	write_places(f, fd);
}

static size_t gather_holders(const struct explain *e, struct holder *out, size_t cap)
{
	size_t n = 0;
	for (size_t i = 0; i < e->nheld; i++) {
		size_t j = 0;
		if (!held_here(&e->held[i]))
			continue;
		while (j < n && out[j].pid != e->held[i].pid)
			j++;
		if (j == n && n == cap)
			continue;
		if (j == n)
			out[n++] = (struct holder){ e->held[i].pid, e->held[i].comm, 0 };
		out[j].size += e->held[i].size;
	}
	for (size_t i = 1; i < n; i++)
		for (size_t k = i; k && out[k - 1].size < out[k].size; k--) {
			struct holder t = out[k];
			out[k] = out[k - 1];
			out[k - 1] = t;
		}
	return n;
}

static void write_held(FILE *f, const struct explain *e)
{
	struct holder holders[256];
	char size[FMT_LEN];
	uint64_t total = held_total(e);
	if (!total)
		return;
	size_t n = gather_holders(e, holders, 256);
	fmt_size(total, size);
	fprintf(f, "%9s  %-5s  deleted files that programs still keep open\n", size, "check");
	fputs("                  run: close or restart ", f);
	for (size_t i = 0; i < n && i < HOLDERS_SHOWN; i++) {
		fmt_size(holders[i].size, size);
		fprintf(f, "%s%s (pid %d, %s)", i ? ", " : "", holders[i].comm, holders[i].pid, size);
	}
	fputs(n > HOLDERS_SHOWN ? ", ... (see NOT SEEN)\n" : "\n", f);
}

static void write_hidden(FILE *f, const struct explain *e)
{
	char size[FMT_LEN];
	uint64_t total = hidden_total(e);
	if (!total)
		return;
	fmt_size(total, size);
	fprintf(f, "%9s  %-5s  files hidden under mount points (see NOT SEEN)\n", size, "check");
	fputs("                  run: (as root) mkdir /tmp/r && mount --bind / /tmp/r, then look under /tmp/r/<mount point>\n", f);
}

static void write_dupes_finding(FILE *f, const struct analysis *a)
{
	char size[FMT_LEN], groups[FMT_LEN];
	if (!a->dupes.wasted)
		return;
	fmt_size(a->dupes.wasted, size);
	fmt_count(a->dupes.ngroups, groups);
	fprintf(f, "%9s  %-5s  extra copies of identical files (%s sets, see DUPLICATE FILES)\n", size, "check", groups);
	fputs("                  run: keep one copy of each; or replace copies with hard links if nothing edits them\n", f);
}

void advice_cleanup(FILE *f, const struct analysis *a, const struct explain *e)
{
	char safe[FMT_LEN], check[FMT_LEN];
	if (!a->nfindings && !held_total(e) && !hidden_total(e) && !a->dupes.wasted)
		return;
	fmt_write_section(f, "WHAT YOU CAN FREE  (safe = rebuilt or downloaded again when needed; check = read first)");
	for (size_t i = 0; i < a->nfindings; i++)
		write_finding(f, &a->findings[i]);
	write_dupes_finding(f, a);
	write_held(f, e);
	write_hidden(f, e);
	fmt_size(a->safe, safe);
	fmt_size(a->check + held_total(e) + hidden_total(e) + a->dupes.wasted, check);
	fprintf(f, "\ntotal      %s safe, %s to check\n", safe, check);
}

void advice_types(FILE *f, const struct analysis *a, uint64_t total)
{
	if (!a->ntypes)
		return;
	fmt_write_section(f, "BY FILE TYPE  (largest extensions)");
	fprintf(f, "%9s %7s %12s  %-12s %s\n", "SIZE", "%", "FILES", "EXTENSION", "KIND");
	for (size_t i = 0; i < a->ntypes && i < TYPE_ROWS; i++) {
		const struct type_stat *t = &a->types[i];
		char size[FMT_LEN], pct[FMT_LEN], count[FMT_LEN], ext[24];
		fmt_size(t->size, size);
		fmt_percent(t->size, total, pct);
		fmt_count(t->count, count);
		snprintf(ext, sizeof ext, "%s%s", *t->ext ? "." : "", *t->ext ? t->ext : "(none)");
		fprintf(f, "%9s %7s %12s  %-12s %s\n", size, pct, count, ext, type_category(t->ext));
	}
}

void advice_stale(FILE *f, const struct analysis *a)
{
	char path[LONG_PATH], size[FMT_LEN], date[FMT_LEN];
	if (!a->nstale)
		return;
	fmt_write_section(f, "UNTOUCHED FOR OVER A YEAR  (big folders with no change anywhere inside)");
	fprintf(f, "%9s  %10s  %s\n", "SIZE", "LAST CHANGE", "PATH");
	for (size_t i = 0; i < a->nstale; i++) {
		fmt_size(node_size(a->stale[i]), size);
		fmt_date(a->stale[i]->mtime, date);
		node_path(a->stale[i], path, sizeof path);
		fprintf(f, "%9s  %11s  ", size, date);
		fmt_write_path(f, path);
		fputs("/\n", f);
	}
}

void advice_dupes(FILE *f, const struct analysis *a)
{
	const struct dupes *d = &a->dupes;
	char path[LONG_PATH], size[FMT_LEN], wasted[FMT_LEN], read[FMT_LEN], files[FMT_LEN], title[128];
	if (!a->dupes_done)
		return;
	fmt_size(d->wasted, wasted);
	fmt_size(d->bytes_read, read);
	fmt_count(d->files_read, files);
	snprintf(title, sizeof title, "DUPLICATE FILES  (same content; %s in extra copies)", wasted);
	fmt_write_section(f, title);
	fprintf(f, "compared   %s candidate files fully, %s read\n", files, read);
	for (size_t i = 0; i < d->ngroups && i < DUPES_LIST; i++) {
		const struct dupe_group *g = &d->groups[i];
		fmt_size(g->size, size);
		fmt_size(g->size * (g->count - 1), wasted);
		fprintf(f, "\n%9s each, %zu copies, %s extra\n", size, g->count, wasted);
		for (size_t j = 0; j < g->count; j++) {
			node_path_in(g->dirs[j], g->files[j], path, sizeof path);
			fputs("           ", f);
			fmt_write_path(f, path);
			fputc('\n', f);
		}
	}
	if (d->ngroups > DUPES_LIST)
		fprintf(f, "\n... and %zu more sets\n", d->ngroups - DUPES_LIST);
}

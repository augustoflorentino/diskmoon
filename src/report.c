#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "fmt.h"
#include "mounts.h"
#include "report.h"

struct ctx {
	FILE *f;
	const struct options *o;
	const char *branch;
	const char *last;
	const char *pipe;
	char prefix[PREFIX_MAX];
	size_t plen;
};

static char *pad_left(char *out, const char *s, int width)
{
	int len = (int)strlen(s);
	for (int i = len; i < width; i++)
		*out++ = ' ';
	memcpy(out, s, (size_t)len);
	return out + len;
}

static char *put(char *out, const char *s, size_t len)
{
	memcpy(out, s, len);
	return out + len;
}

static void write_header(FILE *f, const struct options *o, const struct scan_result *r)
{
	char date[FMT_LEN], files[FMT_LEN], dirs[FMT_LEN], errors[FMT_LEN], host[HOST_NAME_MAX + 1] = "?";
	gethostname(host, sizeof host);
	fmt_datetime(r->started, date);
	fmt_count(r->files, files);
	fmt_count(r->dirs, dirs);
	fmt_count(r->nerrors, errors);
	fprintf(f, "diskmoon %s  (report format %d)\n\n", DISKMOON_VERSION, REPORT_FORMAT);
	fputs("root       ", f);
	fmt_write_path(f, r->root->name);
	fputc('\n', f);
	if (r->source) {
		fputs("source     ", f);
		fmt_write_path(f, r->source);
		fprintf(f, "  (export from %s)\n", r->producer);
		fprintf(f, "scanned    %s\n", r->started ? date : "unknown");
	} else {
		fprintf(f, "host       %s\n", host);
		fprintf(f, "generated  %s\n", date);
		fprintf(f, "took       %.1f s with %d threads\n", r->seconds, r->jobs);
	}
	fprintf(f, "sizes      %s, hard links counted once\n", o->apparent ? "apparent (file length)" : "allocated on disk");
	fprintf(f, "entries    %s files, %s directories, %s unreadable\n", files, dirs, errors);
	if (r->nerrors && geteuid() && !r->source)
		fprintf(f, "\nnote       some paths could not be read; run as root for a complete picture\n");
}

static void write_top_files(FILE *f, const struct tree_stats *s)
{
	char title[64];
	if (!s->top.len)
		return;
	snprintf(title, sizeof title, "LARGEST FILES  (top %zu)", s->top.len);
	fmt_write_section(f, title);
	for (size_t i = 0; i < s->top.len; i++) {
		char size[FMT_LEN], date[FMT_LEN], path[PATH_MAX];
		fmt_size(node_size(s->top.items[i]), size);
		fmt_date(s->top.items[i]->mtime, date);
		node_path(s->top.items[i], path, sizeof path);
		fprintf(f, "%9s  %10s  ", size, date);
		fmt_write_path(f, path);
		fputc('\n', f);
	}
}

static const char *node_label(const struct node *n, char *buf, size_t cap)
{
	buf[0] = '\0';
	if (!(n->flags & (NODE_MOUNT | NODE_SKIPPED | NODE_ERROR | NODE_HARDLINK)))
		return buf;
	const struct mount *m = mounts_get(node_mount(n));
	if (n->flags & NODE_MOUNT)
		snprintf(buf, cap, m && *m->source ? "  [mount: %s %s]" : "  [mount]", m ? m->source : "", m ? m->type : "");
	else if (n->flags & NODE_DUPLICATE)
		snprintf(buf, cap, "  [not scanned: %s, counted under its main mount]", m ? m->source : "same filesystem");
	else if (n->flags & NODE_SKIPPED)
		snprintf(buf, cap, "  [not scanned: %s %s]", m ? m->type : "other filesystem", m ? m->point : "");
	else if (n->flags & NODE_ERROR)
		snprintf(buf, cap, "  [unreadable]");
	else if (n->flags & NODE_HARDLINK)
		snprintf(buf, cap, "  [hard link, counted elsewhere]");
	return buf;
}

static const char *node_suffix(const struct node *n)
{
	if (n->flags & NODE_DIR)
		return node_is_root(n) ? "" : "/";
	return n->flags & NODE_LINK ? "@" : "";
}

static void write_line(struct ctx *c, const struct node *parent, const struct node *n, const char *glyph)
{
	char size[FMT_LEN], pct[FMT_LEN], items[FMT_LEN] = "", date[FMT_LEN], label[PATH_MAX + 64];
	char line[PREFIX_MAX + PATH_MAX * 5 + 512];
	fmt_size(node_size(n), size);
	fmt_percent(node_size(n), parent ? node_size(parent) : node_size(n), pct);
	if (n->flags & NODE_DIR)
		fmt_count(node_dir(n)->items, items);
	fmt_date(n->mtime, date);
	node_label(n, label, sizeof label);
	char *p = pad_left(line, size, 9);
	*p++ = ' ';
	p = pad_left(p, pct, 7);
	*p++ = ' ';
	p = pad_left(p, items, 13);
	p = put(p, "  ", 2);
	p = pad_left(p, date, 10);
	p = put(p, "  ", 2);
	p = put(p, c->prefix, c->plen);
	p = put(p, glyph, strlen(glyph));
	p = fmt_escaped(p, n->name);
	p = put(p, node_suffix(n), strlen(node_suffix(n)));
	p = put(p, label, strlen(label));
	*p++ = '\n';
	fwrite_unlocked(line, 1, (size_t)(p - line), c->f);
}

static bool visible(const struct ctx *c, const struct node *n)
{
	return !c->o->no_files || (n->flags & NODE_DIR);
}

static const struct node *next_visible(const struct ctx *c, const struct node *n)
{
	while (n && !visible(c, n))
		n = node_next(n);
	return n;
}

static void write_rest(struct ctx *c, const struct node *dir, const struct node *from)
{
	uint64_t size = 0, count = 0;
	for (const struct node *n = from; n; n = node_next(n))
		if (visible(c, n)) {
			size += node_size(n);
			count++;
		}
	char sz[FMT_LEN], pct[FMT_LEN], cnt[FMT_LEN];
	fmt_size(size, sz);
	fmt_percent(size, node_size(dir), pct);
	fmt_count(count, cnt);
	fprintf(c->f, "%9s %7s %13s  %10s  %.*s%s[%s smaller entries]\n", sz, pct, "", "", (int)c->plen,
		c->prefix, c->last, cnt);
}

static void push_prefix(struct ctx *c, const char *s)
{
	size_t len = strlen(s);
	if (c->plen + len < PREFIX_MAX)
		memcpy(c->prefix + c->plen, s, len);
	c->plen += len;
	if (c->plen >= PREFIX_MAX)
		c->plen = PREFIX_MAX - 1;
}

static void write_children(struct ctx *c, const struct node *dir, int depth)
{
	if (c->o->max_depth >= 0 && depth >= c->o->max_depth)
		return;
	for (const struct node *n = next_visible(c, node_child(dir)); n; n = next_visible(c, node_next(n))) {
		if (node_size(n) < c->o->min_size) {
			write_rest(c, dir, n);
			return;
		}
		bool last = !next_visible(c, node_next(n));
		write_line(c, dir, n, last ? c->last : c->branch);
		if (!node_child(n))
			continue;
		size_t mark = c->plen;
		push_prefix(c, last ? GLYPH_BLANK : c->pipe);
		write_children(c, n, depth + 1);
		c->plen = mark;
	}
}

static void write_tree(FILE *f, const struct options *o, const struct node *root)
{
	static struct ctx c;
	c = (struct ctx){
		.f = f,
		.o = o,
		.branch = o->ascii ? GLYPH_ASCII_BRANCH : GLYPH_UTF8_BRANCH,
		.last = o->ascii ? GLYPH_ASCII_LAST : GLYPH_UTF8_LAST,
		.pipe = o->ascii ? GLYPH_ASCII_PIPE : GLYPH_UTF8_PIPE,
	};
	fmt_write_section(f, "TREE  (% = share of parent, modified = newest change inside)");
	fprintf(f, "%9s %7s %13s  %10s  %s\n", "SIZE", "%", "ITEMS", "MODIFIED", "PATH");
	write_line(&c, NULL, root, "");
	write_children(&c, root, 0);
}

static void write_errors(FILE *f, const struct scan_result *r)
{
	if (!r->nerrors)
		return;
	fmt_write_section(f, "UNREADABLE PATHS");
	for (size_t i = 0; i < r->nerrors; i++) {
		fmt_write_path(f, r->errors[i].path);
		fprintf(f, ": %s\n", strerror(r->errors[i].err));
	}
}

void report_write(FILE *f, const struct options *o, const struct scan_result *r, const struct tree_stats *s)
{
	write_header(f, o, r);
	write_top_files(f, s);
	write_tree(f, o, r->root);
	write_errors(f, r);
}

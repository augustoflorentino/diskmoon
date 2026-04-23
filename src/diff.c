#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "diff.h"
#include "fmt.h"

struct change {
	const struct node *old;
	const struct node *new;
	int64_t delta;
};

struct changes {
	struct change *items;
	size_t len;
	size_t cap;
};

struct diff_ctx {
	FILE *f;
	const struct options *o;
	struct changes top;
	size_t limit;
	uint64_t grew;
	uint64_t shrank;
	uint64_t added;
	uint64_t removed;
	uint64_t threshold;
	const char *branch;
	const char *last;
	const char *pipe;
	char prefix[PREFIX_MAX];
	size_t plen;
	char path[LONG_PATH];
};

static void *grow_or_die(void *p, size_t size)
{
	p = realloc(p, size);
	if (!p) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	return p;
}

static void changes_add(struct changes *c, struct change ch)
{
	if (c->len == c->cap) {
		c->cap = c->cap ? c->cap * 2 : 16;
		c->items = grow_or_die(c->items, c->cap * sizeof *c->items);
	}
	c->items[c->len++] = ch;
}

static int64_t size_of(const struct node *n)
{
	return n ? (int64_t)node_size(n) : 0;
}

static uint64_t magnitude(int64_t d)
{
	return d < 0 ? (uint64_t)-d : (uint64_t)d;
}

static bool is_dir(const struct node *n)
{
	return n && n->flags & NODE_DIR;
}

static const struct node *either(const struct change *c)
{
	return c->new ? c->new : c->old;
}

static int by_name(const void *a, const void *b)
{
	return strcmp((*(const struct node *const *)a)->name, (*(const struct node *const *)b)->name);
}

static size_t sorted_children(const struct node *dir, const struct node ***out)
{
	size_t n = 0, cap = 0;
	*out = NULL;
	for (const struct node *c = node_child(dir); c; c = node_next(c)) {
		if (n == cap) {
			cap = cap ? cap * 2 : 16;
			*out = grow_or_die(*out, cap * sizeof **out);
		}
		(*out)[n++] = c;
	}
	if (n > 1)
		qsort(*out, n, sizeof **out, by_name);
	return n;
}

static void add_pair(struct changes *out, const struct node *old, const struct node *new)
{
	if (old && new && is_dir(old) != is_dir(new)) {
		add_pair(out, old, NULL);
		add_pair(out, NULL, new);
		return;
	}
	changes_add(out, (struct change){ old, new, size_of(new) - size_of(old) });
}

static void pair_children(const struct node *old, const struct node *new, struct changes *out)
{
	const struct node **a, **b;
	size_t na = sorted_children(old, &a), nb = sorted_children(new, &b), i = 0, j = 0;
	out->len = 0;
	while (i < na || j < nb) {
		int cmp = i == na ? 1 : j == nb ? -1 : strcmp(a[i]->name, b[j]->name);
		add_pair(out, cmp <= 0 ? a[i] : NULL, cmp >= 0 ? b[j] : NULL);
		i += cmp <= 0;
		j += cmp >= 0;
	}
	free(a);
	free(b);
}

static void heap_swap(struct change *h, size_t x, size_t y)
{
	struct change t = h[x];
	h[x] = h[y];
	h[y] = t;
}

static void heap_down(struct change *h, size_t len, size_t i)
{
	for (;;) {
		size_t min = i, l = 2 * i + 1, r = l + 1;
		if (l < len && magnitude(h[l].delta) < magnitude(h[min].delta))
			min = l;
		if (r < len && magnitude(h[r].delta) < magnitude(h[min].delta))
			min = r;
		if (min == i)
			return;
		heap_swap(h, i, min);
		i = min;
	}
}

static void offer(struct diff_ctx *c, struct change ch)
{
	struct changes *t = &c->top;
	if (!c->limit || !ch.delta)
		return;
	if (t->len < c->limit) {
		changes_add(t, ch);
		for (size_t i = t->len - 1; i && magnitude(t->items[(i - 1) / 2].delta) > magnitude(t->items[i].delta);
		     i = (i - 1) / 2)
			heap_swap(t->items, i, (i - 1) / 2);
		return;
	}
	if (magnitude(ch.delta) > magnitude(t->items[0].delta)) {
		t->items[0] = ch;
		heap_down(t->items, t->len, 0);
	}
}

static void account(struct diff_ctx *c, int64_t d)
{
	if (d > 0)
		c->grew += (uint64_t)d;
	else
		c->shrank += magnitude(d);
}

static uint64_t entries_in(const struct node *n)
{
	return 1 + (is_dir(n) ? node_dir(n)->items : 0);
}

static void walk(struct diff_ctx *c, const struct node *old, const struct node *new)
{
	struct changes kids = { 0 };
	int64_t inner = 0;
	pair_children(old, new, &kids);
	for (size_t i = 0; i < kids.len; i++) {
		struct change *k = &kids.items[i];
		inner += k->delta;
		if (k->old && k->new && is_dir(k->old)) {
			walk(c, k->old, k->new);
			continue;
		}
		c->added += k->old ? 0 : entries_in(k->new);
		c->removed += k->new ? 0 : entries_in(k->old);
		account(c, k->delta);
		offer(c, *k);
	}
	account(c, size_of(new) - size_of(old) - inner);
	free(kids.items);
}

static void put_delta(int64_t d, char out[FMT_LEN])
{
	char sz[FMT_LEN];
	fmt_size(magnitude(d), sz);
	out[0] = d > 0 ? '+' : '-';
	size_t at = d != 0, len = strnlen(sz, FMT_LEN - 2);
	memcpy(out + at, sz, len);
	out[at + len] = '\0';
}

static void put_size_or_dash(const struct node *n, char out[FMT_LEN])
{
	if (n)
		fmt_size(node_size(n), out);
	else
		snprintf(out, FMT_LEN, "-");
}

static const char *status_of(const struct change *ch)
{
	if (!ch->old)
		return "new";
	if (!ch->new)
		return "removed";
	return ch->delta > 0 ? "grew" : ch->delta < 0 ? "shrank" : "same";
}

static void write_side(FILE *f, const char *label, const struct scan_result *r)
{
	char date[FMT_LEN];
	fmt_datetime(r->started, date);
	fprintf(f, "%-10s ", label);
	fmt_write_path(f, r->root->name);
	fputs("  (", f);
	if (r->source)
		fmt_write_path(f, r->source);
	else
		fputs("this scan", f);
	fprintf(f, ", %s)\n", r->started ? date : "date unknown");
}

static void write_summary(struct diff_ctx *c, const struct scan_result *old, const struct scan_result *new)
{
	char a[FMT_LEN], b[FMT_LEN], d[FMT_LEN], g[FMT_LEN], s[FMT_LEN], added[FMT_LEN], removed[FMT_LEN], pct[FMT_LEN];
	int64_t delta = size_of(new->root) - size_of(old->root);
	fmt_size(node_size(old->root), a);
	fmt_size(node_size(new->root), b);
	put_delta(delta, d);
	put_delta((int64_t)c->grew, g);
	put_delta(-(int64_t)c->shrank, s);
	fmt_count(c->added, added);
	fmt_count(c->removed, removed);
	fmt_percent(magnitude(delta), node_size(old->root) ? node_size(old->root) : 1, pct);
	fprintf(c->f, "diskmoon %s  (report format %d)\n\n", DISKMOON_VERSION, REPORT_FORMAT);
	write_side(c->f, "old", old);
	write_side(c->f, "new", new);
	fprintf(c->f, "total      %s -> %s  (%s, %s%s)\n", a, b, d, delta < 0 ? "-" : "+", pct);
	fprintf(c->f, "grew       %s\nshrank     %s\n", g, s);
	fprintf(c->f, "entries    %s new, %s removed\n", added, removed);
	if (strcmp(old->root->name, new->root->name))
		fprintf(c->f, "\nnote       the two scans start at different paths; entries are matched by name below them\n");
}

static int by_magnitude(const void *a, const void *b)
{
	const struct change *x = a, *y = b;
	if (magnitude(x->delta) != magnitude(y->delta))
		return magnitude(x->delta) < magnitude(y->delta) ? 1 : -1;
	return strcmp(either(x)->name, either(y)->name);
}

static void write_top(struct diff_ctx *c)
{
	char title[96];
	if (!c->top.len)
		return;
	qsort(c->top.items, c->top.len, sizeof *c->top.items, by_magnitude);
	snprintf(title, sizeof title, "BIGGEST CHANGES  (top %zu: files, and folders that are new or gone as a whole)",
		 c->top.len);
	fmt_write_section(c->f, title);
	fprintf(c->f, "%9s  %-7s  %9s  %9s  %s\n", "CHANGE", "STATUS", "OLD", "NEW", "PATH");
	for (size_t i = 0; i < c->top.len; i++) {
		const struct change *ch = &c->top.items[i];
		char d[FMT_LEN], a[FMT_LEN], b[FMT_LEN];
		put_delta(ch->delta, d);
		put_size_or_dash(ch->old, a);
		put_size_or_dash(ch->new, b);
		node_path(either(ch), c->path, sizeof c->path);
		fprintf(c->f, "%9s  %-7s  %9s  %9s  ", d, status_of(ch), a, b);
		fmt_write_path(c->f, c->path);
		fputs(is_dir(either(ch)) ? "/\n" : "\n", c->f);
	}
}

static void write_change_line(struct diff_ctx *c, const struct change *ch, const char *glyph)
{
	char d[FMT_LEN], a[FMT_LEN], b[FMT_LEN];
	const char *status = ch->old && ch->new ? "" : status_of(ch);
	put_delta(ch->delta, d);
	put_size_or_dash(ch->old, a);
	put_size_or_dash(ch->new, b);
	fprintf(c->f, "%9s  %9s  %9s  %-7s  %.*s%s", d, a, b, status, (int)c->plen, c->prefix, glyph);
	fmt_write_path(c->f, either(ch)->name);
	fputs(is_dir(either(ch)) && !node_is_root(either(ch)) ? "/\n" : "\n", c->f);
}

static size_t shown_count(const struct diff_ctx *c, const struct changes *kids)
{
	size_t n = 0;
	while (n < kids->len && kids->items[n].delta && magnitude(kids->items[n].delta) >= c->threshold)
		n++;
	return n;
}

static void write_rest(struct diff_ctx *c, const struct changes *kids, size_t from)
{
	int64_t net = 0;
	uint64_t count = 0;
	for (size_t i = from; i < kids->len; i++)
		if (kids->items[i].delta) {
			net += kids->items[i].delta;
			count++;
		}
	if (!count)
		return;
	char d[FMT_LEN], n[FMT_LEN];
	put_delta(net, d);
	fmt_count(count, n);
	fprintf(c->f, "%9s  %9s  %9s  %-7s  %.*s%s[%s smaller change%s]\n", d, "", "", "", (int)c->plen, c->prefix,
		c->last, n, count == 1 ? "" : "s");
}

static bool has_rest(const struct changes *kids, size_t from)
{
	for (size_t i = from; i < kids->len; i++)
		if (kids->items[i].delta)
			return true;
	return false;
}

static void write_tree(struct diff_ctx *c, const struct node *old, const struct node *new, int depth)
{
	if (c->o->max_depth >= 0 && depth >= c->o->max_depth)
		return;
	struct changes kids = { 0 };
	pair_children(old, new, &kids);
	if (kids.len > 1)
		qsort(kids.items, kids.len, sizeof *kids.items, by_magnitude);
	size_t shown = shown_count(c, &kids);
	bool rest = has_rest(&kids, shown);
	for (size_t i = 0; i < shown; i++) {
		const struct change *ch = &kids.items[i];
		bool last = i + 1 == shown && !rest;
		write_change_line(c, ch, last ? c->last : c->branch);
		if (!ch->old || !ch->new || !is_dir(ch->old))
			continue;
		size_t mark = c->plen;
		const char *add = last ? GLYPH_BLANK : c->pipe;
		if (c->plen + strlen(add) < PREFIX_MAX) {
			memcpy(c->prefix + c->plen, add, strlen(add));
			c->plen += strlen(add);
		}
		write_tree(c, ch->old, ch->new, depth + 1);
		c->plen = mark;
	}
	write_rest(c, &kids, shown);
	free(kids.items);
}

static uint64_t default_threshold(const struct diff_ctx *c)
{
	if (c->o->min_size)
		return c->o->min_size;
	uint64_t moved = c->grew + c->shrank;
	return moved / 100 ? moved / 100 : 1;
}

static struct diff_ctx *make_ctx(FILE *f, const struct options *o)
{
	struct diff_ctx *c = calloc(1, sizeof *c);
	if (!c) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	c->f = f;
	c->o = o;
	c->limit = (size_t)o->top;
	c->branch = o->ascii ? GLYPH_ASCII_BRANCH : GLYPH_UTF8_BRANCH;
	c->last = o->ascii ? GLYPH_ASCII_LAST : GLYPH_UTF8_LAST;
	c->pipe = o->ascii ? GLYPH_ASCII_PIPE : GLYPH_UTF8_PIPE;
	return c;
}

void diff_write(FILE *f, const struct options *o, const struct scan_result *old, const struct scan_result *new)
{
	struct diff_ctx *c = make_ctx(f, o);
	char thr[FMT_LEN], title[160];
	walk(c, old->root, new->root);
	write_summary(c, old, new);
	write_top(c);
	c->threshold = default_threshold(c);
	fmt_size(c->threshold, thr);
	snprintf(title, sizeof title, "TREE OF CHANGES  (changes of at least %s%s, largest first)", thr,
		 o->min_size ? "" : ", 1% of all growth and shrinking");
	fmt_write_section(f, title);
	fprintf(f, "%9s  %9s  %9s  %-7s  %s\n", "CHANGE", "OLD", "NEW", "STATUS", "PATH");
	struct change root = { old->root, new->root, size_of(new->root) - size_of(old->root) };
	write_change_line(c, &root, "");
	write_tree(c, old->root, new->root, 0);
	free(c->top.items);
	free(c);
}

static void csv_line(struct diff_ctx *c, const struct change *ch)
{
	char escaped[LONG_PATH * 4 + 1];
	*fmt_escaped(escaped, c->path) = '\0';
	fputc('"', c->f);
	for (const char *p = escaped; *p; p++) {
		if (*p == '"')
			fputc('"', c->f);
		fputc(*p, c->f);
	}
	fprintf(c->f, "\",%s,%s,%lld,%lld,%lld\n", status_of(ch), is_dir(either(ch)) ? "dir" : "file",
		(long long)size_of(ch->old), (long long)size_of(ch->new), (long long)ch->delta);
}

static void csv_walk(struct diff_ctx *c, const struct node *old, const struct node *new, size_t len, int depth)
{
	if (c->o->max_depth >= 0 && depth >= c->o->max_depth)
		return;
	struct changes kids = { 0 };
	pair_children(old, new, &kids);
	for (size_t i = 0; i < kids.len; i++) {
		const struct change *ch = &kids.items[i];
		const char *name = either(ch)->name;
		size_t at = len, nlen = strlen(name);
		bool both = ch->old && ch->new;
		if ((both && !ch->delta) || magnitude(ch->delta) < c->o->min_size || at + nlen + 2 > LONG_PATH)
			continue;
		if (c->path[len - 1] != '/')
			c->path[at++] = '/';
		memcpy(c->path + at, name, nlen + 1);
		csv_line(c, ch);
		if (both && is_dir(ch->old))
			csv_walk(c, ch->old, ch->new, at + nlen, depth + 1);
		c->path[len] = '\0';
	}
	free(kids.items);
}

void diff_write_csv(FILE *f, const struct options *o, const struct scan_result *old, const struct scan_result *new)
{
	struct diff_ctx *c = make_ctx(f, o);
	struct change root = { old->root, new->root, size_of(new->root) - size_of(old->root) };
	snprintf(c->path, sizeof c->path, "%s", new->root->name);
	fputs("path,status,kind,old,new,change\n", f);
	csv_line(c, &root);
	csv_walk(c, old->root, new->root, strlen(c->path), 0);
	free(c);
}

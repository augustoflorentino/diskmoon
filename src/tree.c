#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

static void list_reserve(struct node_list *l, size_t need)
{
	if (need <= l->cap)
		return;
	size_t cap = l->cap ? l->cap : 64;
	while (cap < need)
		cap *= 2;
	l->items = realloc(l->items, cap * sizeof *l->items);
	if (!l->items) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	l->cap = cap;
}

static void list_add(struct node_list *l, struct node *n)
{
	list_reserve(l, l->len + 1);
	l->items[l->len++] = n;
}

static void swap(struct node **a, struct node **b)
{
	struct node *t = *a;
	*a = *b;
	*b = t;
}

static void heap_down(struct node **h, size_t len, size_t i)
{
	for (;;) {
		size_t min = i, l = 2 * i + 1, r = l + 1;
		if (l < len && node_size(h[l]) < node_size(h[min]))
			min = l;
		if (r < len && node_size(h[r]) < node_size(h[min]))
			min = r;
		if (min == i)
			return;
		swap(&h[i], &h[min]);
		i = min;
	}
}

static void heap_up(struct node **h, size_t i)
{
	while (i && node_size(h[(i - 1) / 2]) > node_size(h[i])) {
		swap(&h[i], &h[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

static void top_offer(struct node_list *top, size_t limit, struct node *n)
{
	if (top->len < limit) {
		list_add(top, n);
		heap_up(top->items, top->len - 1);
		return;
	}
	if (limit && node_size(n) > node_size(top->items[0])) {
		top->items[0] = n;
		heap_down(top->items, top->len, 0);
	}
}

static int by_size_desc(const void *a, const void *b)
{
	const struct node *x = *(struct node *const *)a, *y = *(struct node *const *)b;
	if (node_size(x) != node_size(y))
		return node_size(x) < node_size(y) ? 1 : -1;
	return strcmp(x->name, y->name);
}

static void sort_children(struct node *dir, struct node_list *scratch)
{
	scratch->len = 0;
	for (struct node *c = node_child(dir); c; c = node_next(c))
		list_add(scratch, c);
	if (scratch->len < 2)
		return;
	qsort(scratch->items, scratch->len, sizeof *scratch->items, by_size_desc);
	node_relink(dir, scratch->items, scratch->len);
}

static void finalize(struct node *n, size_t top, struct tree_stats *s)
{
	if (!(n->flags & NODE_DIR)) {
		top_offer(&s->top, top, n);
		return;
	}
	if (n->flags & NODE_MOUNT)
		list_add(&s->mounts, n);
	struct dir_info *info = node_dir(n);
	for (struct node *c = node_child(n); c; c = node_next(c)) {
		finalize(c, top, s);
		node_set_size(n, node_size(n) + node_size(c));
		info->items += (c->flags & NODE_DIR ? node_dir(c)->items : 0) + 1;
		if (c->mtime > n->mtime)
			n->mtime = c->mtime;
	}
	sort_children(n, &s->scratch);
}

void tree_finalize(struct node *root, size_t top, struct tree_stats *s)
{
	memset(s, 0, sizeof *s);
	finalize(root, top, s);
	free(s->scratch.items);
	s->scratch = (struct node_list){ 0 };
	if (s->top.len)
		qsort(s->top.items, s->top.len, sizeof *s->top.items, by_size_desc);
	if (s->mounts.len)
		qsort(s->mounts.items, s->mounts.len, sizeof *s->mounts.items, by_size_desc);
}

void tree_stats_free(struct tree_stats *s)
{
	free(s->top.items);
	free(s->mounts.items);
	free(s->scratch.items);
	memset(s, 0, sizeof *s);
}

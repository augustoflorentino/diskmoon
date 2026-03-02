#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "links.h"

static int device_of(node_ref dir)
{
	for (const struct node *n = node_at(dir); n; n = node_parent(n))
		if (node_mount(n) >= 0 || node_is_root(n))
			return node_mount(n);
	return -1;
}

static bool same_file(const struct link_ref *x, const struct link_ref *y)
{
	return x->ino == y->ino && (x->dir == y->dir || device_of(x->dir) == device_of(y->dir));
}

static int by_inode(const void *a, const void *b)
{
	const struct link_ref *x = a, *y = b;
	if (x->ino != y->ino)
		return x->ino < y->ino ? -1 : 1;
	int dx = x->dir == y->dir ? 0 : device_of(x->dir), dy = x->dir == y->dir ? 0 : device_of(y->dir);
	if (dx != dy)
		return dx < dy ? -1 : 1;
	return x->node < y->node ? -1 : x->node > y->node;
}

static void sift_down(struct link_ref *a, size_t root, size_t count)
{
	for (size_t child; (child = 2 * root + 1) < count; root = child) {
		if (child + 1 < count && by_inode(&a[child], &a[child + 1]) < 0)
			child++;
		if (by_inode(&a[root], &a[child]) >= 0)
			return;
		struct link_ref tmp = a[root];
		a[root] = a[child];
		a[child] = tmp;
	}
}

static void sort_in_place(struct link_ref *a, size_t count)
{
	for (size_t i = count / 2; i-- > 0;)
		sift_down(a, i, count);
	for (size_t end = count; end-- > 1;) {
		struct link_ref tmp = a[0];
		a[0] = a[end];
		a[end] = tmp;
		sift_down(a, 0, end);
	}
}

static void settle_group(const struct link_ref *group, size_t count)
{
	static char best[PATH_MAX], path[PATH_MAX];
	size_t owner = 0;
	node_path_in(node_at(group[0].dir), node_at(group[0].node), best, sizeof best);
	for (size_t i = 1; i < count; i++) {
		node_path_in(node_at(group[i].dir), node_at(group[i].node), path, sizeof path);
		if (strcmp(path, best) < 0) {
			memcpy(best, path, strlen(path) + 1);
			owner = i;
		}
	}
	for (size_t i = 0; i < count; i++) {
		if (i == owner)
			continue;
		struct node *n = node_at(group[i].node);
		n->flags |= NODE_HARDLINK;
		node_set_size(n, 0);
	}
}

void links_settle(struct link_ref *links, size_t count)
{
	if (count < 2)
		return;
	sort_in_place(links, count);
	for (size_t start = 0, end; start < count; start = end) {
		for (end = start + 1; end < count && same_file(&links[end], &links[start]);)
			end++;
		if (end - start > 1)
			settle_group(links + start, end - start);
	}
}

#include <limits.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "config.h"
#include "node.h"

char *node_space;
static size_t space_cap;
static atomic_size_t space_used = NODE_UNIT * 16;

bool node_space_init(void)
{
	if (node_space)
		return true;
	for (size_t cap = NODE_SPACE_MAX; cap >= NODE_SPACE_MIN; cap /= 2) {
		void *p = mmap(NULL, cap, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
		if (p != MAP_FAILED) {
			node_space = p;
			space_cap = cap;
			return true;
		}
	}
	return false;
}

size_t node_space_used(void)
{
	return atomic_load(&space_used);
}

static void arena_refill(struct arena *a, size_t size)
{
	size_t chunk = size > ARENA_CHUNK ? size : ARENA_CHUNK;
	size_t at = atomic_fetch_add(&space_used, chunk);
	if (at + chunk > space_cap) {
		fputs("diskmoon: tree is larger than the reserved node space\n", stderr);
		exit(2);
	}
	a->cur = node_space + at;
	a->end = a->cur + chunk;
}

static void *arena_alloc(struct arena *a, size_t size)
{
	size = (size + NODE_UNIT - 1) & ~(size_t)(NODE_UNIT - 1);
	if (!a->cur || (size_t)(a->end - a->cur) < size)
		arena_refill(a, size);
	void *p = a->cur;
	a->cur += size;
	return p;
}

struct node *node_new(struct arena *a, struct node *parent, const char *name, bool dir, uint64_t size)
{
	size_t len = strlen(name) + 1;
	bool wide = !dir && size >= UINT32_MAX;
	size_t head = dir ? sizeof(struct dir_info) + NODE_DIR_HIGH : wide ? NODE_FILE_WIDE : 0;
	char *p = arena_alloc(a, head + offsetof(struct node, name) + len);
	struct node *n = (struct node *)(p + head);
	memset(p, 0, head + offsetof(struct node, name));
	n->flags = dir ? NODE_DIR : 0;
	if (wide)
		n->size_low = UINT32_MAX;
	node_set_size(n, size);
	if (dir) {
		node_dir(n)->mount = -1;
		node_dir(n)->parent = node_ref_of(parent);
	}
	memcpy(n->name, name, len);
	return n;
}

void node_push_child(struct node *dir, struct node *n)
{
	struct dir_info *info = node_dir(dir);
	n->next = info->child ? info->child : node_ref_of(dir) | NODE_LAST;
	info->child = node_ref_of(n);
}

void node_relink(struct node *dir, struct node **children, size_t count)
{
	for (size_t i = 0; i + 1 < count; i++)
		children[i]->next = node_ref_of(children[i + 1]);
	if (count)
		children[count - 1]->next = node_ref_of(dir) | NODE_LAST;
	node_dir(dir)->child = count ? node_ref_of(children[0]) : 0;
}

struct node *node_parent(const struct node *n)
{
	if (n->flags & NODE_DIR)
		return node_at(node_dir(n)->parent);
	while (n->next && !(n->next & NODE_LAST))
		n = node_at(n->next);
	return n->next ? node_at(n->next & ~NODE_LAST) : NULL;
}

static size_t append(char *buf, size_t len, size_t cap, const char *s)
{
	size_t add = strlen(s);
	if (len + add >= cap)
		add = cap - len - 1;
	memcpy(buf + len, s, add);
	return len + add;
}

static size_t separator(const struct node *child)
{
	const struct node *parent = node_parent(child);
	if (!parent)
		return 0;
	size_t len = strlen(parent->name);
	return !len || parent->name[len - 1] != '/';
}

size_t node_path(const struct node *n, char *buf, size_t cap)
{
	size_t need = 0;
	for (const struct node *p = n; p; p = node_parent(p))
		need += strlen(p->name) + separator(p);
	size_t at = need < cap ? need : cap - 1;
	buf[at] = '\0';
	size_t pos = need;
	for (const struct node *p = n; p; p = node_parent(p)) {
		size_t len = strlen(p->name);
		pos -= len;
		for (size_t i = 0; i < len; i++)
			if (pos + i < at)
				buf[pos + i] = p->name[i];
		if (!separator(p))
			continue;
		if (--pos < at)
			buf[pos] = '/';
	}
	return at;
}

size_t node_path_in(const struct node *dir, const struct node *n, char *buf, size_t cap)
{
	size_t len = node_path(dir, buf, cap);
	if (len && buf[len - 1] != '/')
		len = append(buf, len, cap, "/");
	len = append(buf, len, cap, n->name);
	buf[len] = '\0';
	return len;
}

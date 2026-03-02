#ifndef DISKMOON_NODE_H
#define DISKMOON_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum node_flag {
	NODE_DIR = 1 << 0,
	NODE_MOUNT = 1 << 1,
	NODE_SKIPPED = 1 << 2,
	NODE_ERROR = 1 << 3,
	NODE_LINK = 1 << 4,
	NODE_HARDLINK = 1 << 5,
	NODE_DUPLICATE = 1 << 6,
	NODE_CLAIMED = 1 << 7,
};

#define NODE_UNIT 2
#define NODE_LAST 0x80000000u

typedef uint32_t node_ref;

struct __attribute__((packed)) dir_info {
	node_ref child;
	node_ref parent;
	uint32_t items;
	int16_t mount;
};

struct __attribute__((packed)) node {
	node_ref next;
	uint32_t mtime;
	uint32_t size_low;
	uint8_t flags;
	char name[];
};

struct arena {
	char *cur;
	char *end;
};

extern char *node_space;

bool node_space_init(void);
size_t node_space_used(void);
struct node *node_new(struct arena *a, struct node *parent, const char *name, bool dir, uint64_t size);
void node_push_child(struct node *dir, struct node *n);
void node_relink(struct node *dir, struct node **children, size_t count);
struct node *node_parent(const struct node *n);
size_t node_path(const struct node *n, char *buf, size_t cap);
size_t node_path_in(const struct node *dir, const struct node *n, char *buf, size_t cap);

static inline struct node *node_at(node_ref r)
{
	return r ? (struct node *)(node_space + (size_t)r * NODE_UNIT) : NULL;
}

static inline node_ref node_ref_of(const struct node *n)
{
	return n ? (node_ref)(((const char *)n - node_space) / NODE_UNIT) : 0;
}

#define NODE_DIR_HIGH 4
#define NODE_FILE_WIDE 8

static inline uint64_t node_size(const struct node *n)
{
	if (n->flags & NODE_DIR) {
		uint32_t high;
		memcpy(&high, (const char *)n - NODE_DIR_HIGH, sizeof high);
		return (uint64_t)high << 32 | n->size_low;
	}
	if (n->size_low != UINT32_MAX)
		return n->size_low;
	uint64_t full;
	memcpy(&full, (const char *)n - NODE_FILE_WIDE, sizeof full);
	return full;
}

static inline void node_set_size(struct node *n, uint64_t size)
{
	if (n->flags & NODE_DIR) {
		uint32_t high = (uint32_t)(size >> 32);
		memcpy((char *)n - NODE_DIR_HIGH, &high, sizeof high);
		n->size_low = (uint32_t)size;
	} else if (n->size_low == UINT32_MAX) {
		memcpy((char *)n - NODE_FILE_WIDE, &size, sizeof size);
	} else {
		n->size_low = size < UINT32_MAX ? (uint32_t)size : UINT32_MAX - 1;
	}
}

static inline struct dir_info *node_dir(const struct node *n)
{
	return (struct dir_info *)((char *)n - NODE_DIR_HIGH - sizeof(struct dir_info));
}

static inline bool node_is_root(const struct node *n)
{
	return !n->next;
}

static inline struct node *node_next(const struct node *n)
{
	return n->next & NODE_LAST ? NULL : node_at(n->next);
}

static inline struct node *node_child(const struct node *n)
{
	return n->flags & NODE_DIR ? node_at(node_dir(n)->child) : NULL;
}

static inline int node_mount(const struct node *n)
{
	return n->flags & NODE_DIR ? node_dir(n)->mount : -1;
}

#endif

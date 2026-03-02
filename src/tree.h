#ifndef DISKMOON_TREE_H
#define DISKMOON_TREE_H

#include <stddef.h>

#include "node.h"

struct node_list {
	struct node **items;
	size_t len;
	size_t cap;
};

struct tree_stats {
	struct node_list top;
	struct node_list mounts;
	struct node_list scratch;
};

void tree_finalize(struct node *root, size_t top, struct tree_stats *s);
void tree_stats_free(struct tree_stats *s);

#endif

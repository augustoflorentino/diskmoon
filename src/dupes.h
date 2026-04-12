#ifndef DISKMOON_DUPES_H
#define DISKMOON_DUPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "node.h"

struct dupe_group {
	uint64_t size;
	const struct node **files;
	const struct node **dirs;
	size_t count;
};

struct dupes {
	struct dupe_group *groups;
	size_t ngroups;
	uint64_t wasted;
	uint64_t files_read;
	uint64_t bytes_read;
};

void dupes_find(const struct node *root, uint64_t min_size, int jobs, bool progress, struct dupes *d);
void dupes_free(struct dupes *d);

#endif

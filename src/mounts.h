#ifndef DISKMOON_MOUNTS_H
#define DISKMOON_MOUNTS_H

#include <stdbool.h>
#include <stdint.h>

struct mount {
	int id;
	int parent;
	uint64_t dev;
	uint64_t block;
	char *point;
	char *source;
	char *type;
	bool real;
	bool bind;
	bool subvolume;
};

int mounts_load(void);
int mounts_find(uint64_t dev);
int mounts_find_point(const char *path);
int mounts_find_id(int id);
int mounts_add_subvolume(int parent, uint64_t dev, const char *point);
const struct mount *mounts_get(int index);
int mounts_count(void);
int mounts_add_imported(uint64_t dev, const char *point);

#endif

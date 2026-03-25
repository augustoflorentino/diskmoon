#ifndef DISKMOON_EXPLAIN_H
#define DISKMOON_EXPLAIN_H

#include <stddef.h>
#include <stdint.h>

struct held_file {
	uint64_t dev;
	uint64_t ino;
	uint64_t size;
	int pid;
	char comm[16];
	char *path;
};

struct hidden_dir {
	int mount;
	int covered_by;
	uint64_t size;
	uint64_t files;
	uint64_t largest_size;
	char *largest;
	int err;
};

struct explain {
	struct held_file *held;
	size_t nheld;
	size_t capheld;
	struct hidden_dir *hidden;
	size_t nhidden;
	size_t caphidden;
};

void explain_collect(struct explain *e, const int *mounts, size_t nmounts);
uint64_t explain_held(const struct explain *e, uint64_t dev, size_t *count);
void explain_free(struct explain *e);

#endif

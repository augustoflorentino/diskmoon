#ifndef DISKMOON_SCAN_H
#define DISKMOON_SCAN_H

#include <stddef.h>
#include <stdint.h>

#include "node.h"
#include "options.h"

struct scan_error {
	char *path;
	int err;
};

struct scan_result {
	struct node *root;
	uint64_t files;
	uint64_t dirs;
	struct scan_error *errors;
	size_t nerrors;
	double seconds;
	int jobs;
	int64_t started;
	const char *source;
	char producer[64];
};

int scan_run(const struct options *o, struct scan_result *r);
void scan_result_free(struct scan_result *r);

#endif

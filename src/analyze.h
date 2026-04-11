#ifndef DISKMOON_ANALYZE_H
#define DISKMOON_ANALYZE_H

#include <stddef.h>
#include <stdint.h>

#include "rules.h"
#include "scan.h"

#define FINDING_PATHS 5
#define TYPE_SLOTS 4096
#define HOT_PATH_MAX 16
#define STALE_MAX 15

struct finding {
	const struct rule *rule;
	uint64_t size;
	size_t count;
	const struct node *paths[FINDING_PATHS];
	char home[64];
};

struct type_stat {
	char ext[16];
	uint64_t size;
	uint64_t count;
};

struct analysis {
	struct finding *findings;
	size_t nfindings;
	size_t capfindings;
	uint64_t safe;
	uint64_t check;
	struct type_stat *types;
	size_t ntypes;
	const struct node *hot[HOT_PATH_MAX];
	size_t nhot;
	const struct node *stale[STALE_MAX];
	size_t nstale;
	uint64_t stale_floor;
	int64_t now;
};

void analyze_run(const struct scan_result *r, struct analysis *a);
void analyze_free(struct analysis *a);
const char *type_category(const char *ext);

#endif

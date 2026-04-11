#ifndef DISKMOON_RULES_H
#define DISKMOON_RULES_H

#include <stdbool.h>

enum rule_kind { RULE_PATH, RULE_NAME };
enum safety { SAFETY_SAFE, SAFETY_CHECK };

struct rule {
	enum rule_kind kind;
	const char *match;
	const char *marker;
	bool marker_in_parent;
	unsigned stale_days;
	enum safety safety;
	bool keep;
	const char *what;
	const char *how;
};

extern const struct rule cleanup_rules[];

#endif

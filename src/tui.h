#ifndef DISKMOON_TUI_H
#define DISKMOON_TUI_H

#include "analyze.h"
#include "explain.h"
#include "options.h"
#include "scan.h"
#include "tree.h"

int tui_run(const struct options *o, const struct scan_result *r, const struct tree_stats *s, const struct explain *e,
	    const struct analysis *a);

#endif

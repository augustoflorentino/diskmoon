#ifndef DISKMOON_REPORT_H
#define DISKMOON_REPORT_H

#include <stdio.h>

#include "options.h"
#include "scan.h"
#include "tree.h"

void report_write(FILE *f, const struct options *o, const struct scan_result *r, const struct tree_stats *s);

#endif

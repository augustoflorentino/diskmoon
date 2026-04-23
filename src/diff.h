#ifndef DISKMOON_DIFF_H
#define DISKMOON_DIFF_H

#include <stdio.h>

#include "options.h"
#include "scan.h"

void diff_write(FILE *f, const struct options *o, const struct scan_result *old, const struct scan_result *new);
void diff_write_csv(FILE *f, const struct options *o, const struct scan_result *old, const struct scan_result *new);

#endif

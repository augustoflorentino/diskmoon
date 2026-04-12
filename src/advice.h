#ifndef DISKMOON_ADVICE_H
#define DISKMOON_ADVICE_H

#include <stdio.h>

#include "analyze.h"
#include "explain.h"
#include "scan.h"

void advice_summary(FILE *f, const struct scan_result *r, const struct explain *e, const struct analysis *a);
void advice_cleanup(FILE *f, const struct analysis *a, const struct explain *e);
void advice_types(FILE *f, const struct analysis *a, uint64_t total);
void advice_stale(FILE *f, const struct analysis *a);
void advice_dupes(FILE *f, const struct analysis *a);

#endif

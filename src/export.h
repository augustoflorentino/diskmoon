#ifndef DISKMOON_EXPORT_H
#define DISKMOON_EXPORT_H

#include <stdio.h>

#include "options.h"
#include "scan.h"

void export_ncdu(FILE *f, const struct options *o, const struct scan_result *r);
void export_csv(FILE *f, const struct options *o, const struct scan_result *r);

#endif

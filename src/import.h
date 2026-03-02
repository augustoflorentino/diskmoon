#ifndef DISKMOON_IMPORT_H
#define DISKMOON_IMPORT_H

#include "options.h"
#include "scan.h"

int import_ncdu(const struct options *o, const char *file, struct scan_result *r);

#endif

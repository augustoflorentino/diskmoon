#ifndef DISKMOON_HISTORY_H
#define DISKMOON_HISTORY_H

#include <stdbool.h>
#include <stddef.h>

#include "options.h"
#include "scan.h"

bool history_new_path(const char *root, char *out, size_t cap);
bool history_latest(const char *root, const char *before, char *out, size_t cap);
bool history_save(const struct options *o, const struct scan_result *r, char *path, size_t cap);

#endif

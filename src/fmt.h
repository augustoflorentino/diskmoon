#ifndef DISKMOON_FMT_H
#define DISKMOON_FMT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define FMT_LEN 32

void fmt_size(uint64_t bytes, char out[FMT_LEN]);
void fmt_count(uint64_t n, char out[FMT_LEN]);
void fmt_percent(uint64_t part, uint64_t whole, char out[FMT_LEN]);
void fmt_date(int64_t t, char out[FMT_LEN]);
void fmt_datetime(int64_t t, char out[FMT_LEN]);
bool parse_size(const char *s, uint64_t *out);
char *fmt_escaped(char *out, const char *s);
void fmt_write_path(FILE *f, const char *path);
void fmt_write_section(FILE *f, const char *title);

#endif

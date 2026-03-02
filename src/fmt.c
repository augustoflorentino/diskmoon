#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "fmt.h"

#define DATE_BUCKET 900

__extension__ typedef unsigned __int128 u128;
#define DATE_CACHE 4096

struct date_slot {
	int64_t bucket;
	char text[11];
};

static struct date_slot date_cache[DATE_CACHE];

static char *put_uint(char *out, uint64_t v)
{
	char tmp[24];
	int n = 0;
	do
		tmp[n++] = (char)('0' + v % 10);
	while ((v /= 10));
	while (n)
		*out++ = tmp[--n];
	return out;
}

static void put_tenths(char *out, uint64_t tenths, const char *suffix)
{
	out = put_uint(out, tenths / 10);
	*out++ = '.';
	*out++ = (char)('0' + tenths % 10);
	strcpy(out, suffix);
}

void fmt_size(uint64_t bytes, char out[FMT_LEN])
{
	static const char *const units[] = { "K", "M", "G", "T", "P", "E" };
	if (bytes < 1024) {
		strcpy(put_uint(out, bytes), "B");
		return;
	}
	u128 div = 1024;
	int u = 0;
	while (bytes >= div * 1024 && u < 5) {
		div *= 1024;
		u++;
	}
	put_tenths(out, (uint64_t)(((u128)bytes * 10 + div / 2) / div), units[u]);
}

void fmt_count(uint64_t n, char out[FMT_LEN])
{
	char digits[24];
	int len = (int)(put_uint(digits, n) - digits);
	int j = 0;
	for (int i = 0; i < len; i++) {
		if (i && (len - i) % 3 == 0)
			out[j++] = ',';
		out[j++] = digits[i];
	}
	out[j] = '\0';
}

void fmt_percent(uint64_t part, uint64_t whole, char out[FMT_LEN])
{
	if (!whole) {
		strcpy(out, "-");
		return;
	}
	put_tenths(out, (uint64_t)(((u128)part * 1000 + whole / 2) / whole), "%");
}

static void fmt_time(int64_t t, const char *pattern, char out[FMT_LEN])
{
	time_t tt = (time_t)t;
	struct tm tm;
	if (t <= 0 || !localtime_r(&tt, &tm) || !strftime(out, FMT_LEN, pattern, &tm))
		strcpy(out, "-");
}

void fmt_date(int64_t t, char out[FMT_LEN])
{
	int64_t bucket = t / DATE_BUCKET;
	struct date_slot *slot = &date_cache[(uint64_t)bucket % DATE_CACHE];
	if (t <= 0 || slot->bucket != bucket || !slot->text[0]) {
		fmt_time(t, "%Y-%m-%d", out);
		if (t > 0) {
			slot->bucket = bucket;
			snprintf(slot->text, sizeof slot->text, "%s", out);
		}
		return;
	}
	memcpy(out, slot->text, sizeof slot->text);
}

void fmt_datetime(int64_t t, char out[FMT_LEN])
{
	fmt_time(t, "%Y-%m-%d %H:%M:%S", out);
}

bool parse_size(const char *s, uint64_t *out)
{
	static const char units[] = "BKMGTPE";
	char *end;
	errno = 0;
	double v = strtod(s, &end);
	if (end == s || v < 0 || errno)
		return false;
	uint64_t mult = 1;
	if (*end) {
		const char *u = strchr(units, toupper((unsigned char)*end));
		if (!u)
			return false;
		mult = 1ull << (10 * (u - units));
		end++;
		if (*end == 'i')
			end++;
		if (*end == 'B' || *end == 'b')
			end++;
	}
	if (*end)
		return false;
	*out = (uint64_t)(v * (double)mult);
	return true;
}

char *fmt_escaped(char *out, const char *s)
{
	static const char hex[] = "0123456789abcdef";
	for (const unsigned char *c = (const unsigned char *)s; *c; c++) {
		if (*c >= 0x20 && *c != 0x7f && *c != '\\') {
			*out++ = (char)*c;
			continue;
		}
		*out++ = '\\';
		if (*c == '\n' || *c == '\t' || *c == '\\') {
			*out++ = *c == '\n' ? 'n' : *c == '\t' ? 't' : '\\';
			continue;
		}
		*out++ = 'x';
		*out++ = hex[*c >> 4];
		*out++ = hex[*c & 15];
	}
	return out;
}

void fmt_write_path(FILE *f, const char *path)
{
	char buf[LONG_PATH * 4 + 1];
	*fmt_escaped(buf, path) = '\0';
	fputs(buf, f);
}

void fmt_write_section(FILE *f, const char *title)
{
	fprintf(f, "\n%s\n%s\n%s\n", RULE, title, RULE);
}

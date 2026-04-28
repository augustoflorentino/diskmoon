#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "export.h"
#include "history.h"

#define OUTPUT_BUFFER_SIZE (1u << 20)

static bool base_dir(char *out, size_t cap)
{
	const char *data = getenv("XDG_DATA_HOME"), *home = getenv("HOME");
	if (data && *data)
		return (size_t)snprintf(out, cap, "%s/diskmoon/scans", data) < cap;
	return home && (size_t)snprintf(out, cap, "%s/.local/share/diskmoon/scans", home) < cap;
}

static bool root_dir(const char *root, char *out, size_t cap)
{
	if (!base_dir(out, cap))
		return false;
	size_t len = strlen(out);
	if (len + 2 >= cap)
		return false;
	out[len++] = '/';
	for (const char *c = root; *c; c++) {
		const char *piece = *c == '/' ? "%2F" : *c == '%' ? "%25" : NULL;
		size_t add = piece ? 3 : 1;
		if (len + add >= cap)
			return false;
		memcpy(out + len, piece ? piece : c, add);
		len += add;
	}
	out[len] = '\0';
	return true;
}

static bool make_dirs(char *path)
{
	for (char *p = path + 1; *p; p++) {
		if (*p != '/')
			continue;
		*p = '\0';
		bool ok = !mkdir(path, 0700) || errno == EEXIST;
		*p = '/';
		if (!ok)
			return false;
	}
	return !mkdir(path, 0700) || errno == EEXIST;
}

bool history_new_path(const char *root, char *out, size_t cap)
{
	char dir[4096], stamp[32];
	time_t now = time(NULL);
	struct tm tm;
	localtime_r(&now, &tm);
	strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tm);
	if (!root_dir(root, dir, sizeof dir) || !make_dirs(dir))
		return false;
	return (size_t)snprintf(out, cap, "%s/%s.json", dir, stamp) < cap;
}

bool history_latest(const char *root, const char *before, char *out, size_t cap)
{
	char dir[4096], best[256] = "";
	const char *limit = before ? strrchr(before, '/') : NULL;
	limit = limit ? limit + 1 : before;
	if (!root_dir(root, dir, sizeof dir))
		return false;
	DIR *d = opendir(dir);
	if (!d)
		return false;
	struct dirent *ent;
	while ((ent = readdir(d))) {
		size_t len = strlen(ent->d_name);
		if (len > 5 && !strcmp(ent->d_name + len - 5, ".json") && strcmp(ent->d_name, best) > 0 &&
		    (!limit || strcmp(ent->d_name, limit) < 0) &&
		    len < sizeof best)
			memcpy(best, ent->d_name, len + 1);
	}
	closedir(d);
	return best[0] && (size_t)snprintf(out, cap, "%s/%s", dir, best) < cap;
}

bool history_save(const struct options *o, const struct scan_result *r, char *path, size_t cap)
{
	if (!path[0] && !history_new_path(r->root->name, path, cap))
		return false;
	FILE *f = fopen(path, "w");
	if (!f)
		return false;
	setvbuf(f, NULL, _IOFBF, OUTPUT_BUFFER_SIZE);
	export_ncdu(f, o, r);
	return !fclose(f);
}

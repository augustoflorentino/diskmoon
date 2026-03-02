#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "mounts.h"

#define MAX_FIELDS 64

static struct mount *table;
static int count;
static int capacity;

static const char *const disk_types[] = {
	"ext2", "ext3", "ext4", "xfs", "btrfs", "zfs", "f2fs", "bcachefs",
	"jfs", "reiserfs", "ntfs", "ntfs3", "vfat", "exfat", "hfsplus", NULL,
};

static bool is_disk(const struct mount *m)
{
	if (!strncmp(m->source, "/dev/", 5))
		return true;
	for (int i = 0; disk_types[i]; i++)
		if (!strcmp(m->type, disk_types[i]))
			return true;
	return false;
}

static char *unescape(const char *s)
{
	char *out = malloc(strlen(s) + 1);
	if (!out)
		return NULL;
	char *o = out;
	while (*s) {
		unsigned code;
		if (s[0] == '\\' && sscanf(s + 1, "%3o", &code) == 1) {
			*o++ = (char)code;
			s += 4;
			continue;
		}
		*o++ = *s++;
	}
	*o = '\0';
	return out;
}

static int split(char *line, char **fields)
{
	int n = 0;
	for (char *tok = strtok(line, " \n"); tok && n < MAX_FIELDS; tok = strtok(NULL, " \n"))
		fields[n++] = tok;
	return n;
}

static int find_separator(char **fields, int n)
{
	for (int i = 6; i < n; i++)
		if (!strcmp(fields[i], "-"))
			return i;
	return -1;
}

static uint64_t block_of(const struct mount *m)
{
	struct stat st;
	if (m->source[0] == '/' && !stat(m->source, &st) && S_ISBLK(st.st_mode))
		return st.st_rdev;
	return m->dev;
}

static void append(struct mount m)
{
	if (count == capacity) {
		capacity = capacity ? capacity * 2 : 64;
		table = realloc(table, (size_t)capacity * sizeof *table);
		if (!table) {
			fputs("diskmoon: out of memory\n", stderr);
			exit(2);
		}
	}
	m.real = is_disk(&m);
	m.block = block_of(&m);
	table[count++] = m;
}

static void parse_line(char *line)
{
	char *fields[MAX_FIELDS];
	int n = split(line, fields);
	int sep = find_separator(fields, n);
	unsigned major, minor;
	if (sep < 0 || sep + 2 >= n || sscanf(fields[2], "%u:%u", &major, &minor) != 2)
		return;
	struct mount m = {
		.id = atoi(fields[0]),
		.parent = atoi(fields[1]),
		.dev = makedev(major, minor),
		.point = unescape(fields[4]),
		.source = unescape(fields[sep + 2]),
		.type = strdup(fields[sep + 1]),
		.bind = strcmp(fields[3], "/") != 0,
	};
	if (m.point && m.source && m.type)
		append(m);
}

int mounts_load(void)
{
	FILE *f = fopen("/proc/self/mountinfo", "r");
	if (!f)
		return -1;
	char *line = NULL;
	size_t cap = 0;
	while (getline(&line, &cap, f) > 0)
		parse_line(line);
	free(line);
	fclose(f);
	return 0;
}

int mounts_find(uint64_t dev)
{
	int found = -1;
	for (int i = 0; i < count; i++)
		if (table[i].dev == dev && (found < 0 || (table[found].bind && !table[i].bind)))
			found = i;
	return found;
}

int mounts_find_point(const char *path)
{
	for (int i = count - 1; i >= 0; i--)
		if (!strcmp(table[i].point, path))
			return i;
	return -1;
}

int mounts_find_id(int id)
{
	for (int i = 0; i < count; i++)
		if (table[i].id == id && !table[i].subvolume)
			return i;
	return -1;
}

int mounts_add_subvolume(int parent, uint64_t dev, const char *point)
{
	struct mount m = table[parent];
	m.dev = dev;
	m.point = strdup(point);
	m.source = strdup(m.source);
	m.type = strdup(m.type);
	m.subvolume = true;
	append(m);
	table[count - 1].real = table[parent].real;
	return count - 1;
}

int mounts_add_imported(uint64_t dev, const char *point)
{
	for (int i = 0; i < count; i++)
		if (table[i].dev == dev && !strcmp(table[i].point, point))
			return i;
	append((struct mount){ .id = -1, .parent = -1, .dev = dev, .point = strdup(point), .source = strdup(""),
			       .type = strdup("") });
	return count - 1;
}

int mounts_count(void)
{
	return count;
}

const struct mount *mounts_get(int index)
{
	return index >= 0 && index < count ? &table[index] : NULL;
}

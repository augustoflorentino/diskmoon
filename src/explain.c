#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "explain.h"
#include "mounts.h"

#define DELETED_SUFFIX " (deleted)"
#define OPEN_DIR_FLAGS (O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC)

static void *grow(void *items, size_t *cap, size_t size)
{
	*cap = *cap ? *cap * 2 : 64;
	items = realloc(items, *cap * size);
	if (!items) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	return items;
}

static bool strip_deleted(char *target)
{
	size_t len = strlen(target), suffix = strlen(DELETED_SUFFIX);
	if (len < suffix || strcmp(target + len - suffix, DELETED_SUFFIX))
		return false;
	target[len - suffix] = '\0';
	return true;
}

static void add_held(struct explain *e, const struct stat *st, int pid, const char *comm, const char *path)
{
	if (e->nheld == e->capheld)
		e->held = grow(e->held, &e->capheld, sizeof *e->held);
	struct held_file *h = &e->held[e->nheld++];
	*h = (struct held_file){ st->st_dev, st->st_ino, (uint64_t)st->st_blocks * 512, pid, "", strdup(path) };
	snprintf(h->comm, sizeof h->comm, "%s", comm);
}

static void scan_links(struct explain *e, int pid, const char *kind, const char *comm)
{
	char path[64];
	snprintf(path, sizeof path, "/proc/%d/%s", pid, kind);
	int dfd = open(path, OPEN_DIR_FLAGS);
	DIR *d = dfd < 0 ? NULL : fdopendir(dfd);
	if (!d) {
		if (dfd >= 0)
			close(dfd);
		return;
	}
	struct dirent *ent;
	while ((ent = readdir(d))) {
		char target[PATH_MAX];
		struct stat st;
		ssize_t n = readlinkat(dfd, ent->d_name, target, sizeof target - 1);
		if (ent->d_name[0] == '.' || n < 0)
			continue;
		target[n] = '\0';
		if (strip_deleted(target) && !fstatat(dfd, ent->d_name, &st, 0) && S_ISREG(st.st_mode) && !st.st_nlink)
			add_held(e, &st, pid, comm, target);
	}
	closedir(d);
}

static void read_comm(int pid, char *out, size_t cap)
{
	char path[64];
	snprintf(path, sizeof path, "/proc/%d/comm", pid);
	FILE *f = fopen(path, "r");
	out[0] = '\0';
	if (f && fgets(out, (int)cap, f))
		out[strcspn(out, "\n")] = '\0';
	if (f)
		fclose(f);
}

static int by_inode(const void *a, const void *b)
{
	const struct held_file *x = a, *y = b;
	if (x->dev != y->dev)
		return x->dev < y->dev ? -1 : 1;
	if (x->ino != y->ino)
		return x->ino < y->ino ? -1 : 1;
	return x->pid - y->pid;
}

static int by_size(const void *a, const void *b)
{
	const struct held_file *x = a, *y = b;
	return x->size == y->size ? 0 : x->size < y->size ? 1 : -1;
}

static void dedupe_held(struct explain *e)
{
	if (!e->nheld)
		return;
	qsort(e->held, e->nheld, sizeof *e->held, by_inode);
	size_t out = 1;
	for (size_t i = 1; i < e->nheld; i++) {
		if (e->held[i].dev == e->held[out - 1].dev && e->held[i].ino == e->held[out - 1].ino) {
			free(e->held[i].path);
			continue;
		}
		e->held[out++] = e->held[i];
	}
	e->nheld = out;
	qsort(e->held, e->nheld, sizeof *e->held, by_size);
}

static void collect_held(struct explain *e)
{
	DIR *proc = opendir("/proc");
	if (!proc)
		return;
	struct dirent *ent;
	while ((ent = readdir(proc))) {
		if (!isdigit((unsigned char)ent->d_name[0]))
			continue;
		int pid = atoi(ent->d_name);
		char comm[16];
		read_comm(pid, comm, sizeof comm);
		scan_links(e, pid, "fd", comm);
		scan_links(e, pid, "map_files", comm);
	}
	closedir(proc);
	dedupe_held(e);
}

static void note_largest(struct hidden_dir *h, const char *dir, const char *name, uint64_t size)
{
	if (size <= h->largest_size)
		return;
	char path[PATH_MAX + NAME_MAX + 2];
	snprintf(path, sizeof path, "%s%s%s", dir, *dir ? "/" : "", name);
	free(h->largest);
	h->largest = strdup(path);
	h->largest_size = size;
}

static void walk(int dfd, struct hidden_dir *h, const char *dir, int depth)
{
	char buf[DENTS_BUFFER / 4];
	ssize_t got;
	while ((got = getdents64(dfd, buf, sizeof buf)) > 0)
		for (ssize_t off = 0; off < got;) {
			struct dirent64 *ent = (struct dirent64 *)(buf + off);
			struct stat st;
			off += ent->d_reclen;
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..") ||
			    fstatat(dfd, ent->d_name, &st, AT_SYMLINK_NOFOLLOW))
				continue;
			h->size += (uint64_t)st.st_blocks * 512;
			h->files++;
			if (!S_ISDIR(st.st_mode)) {
				note_largest(h, dir, ent->d_name, (uint64_t)st.st_blocks * 512);
				continue;
			}
			int child = depth < FD_DEPTH_MAX ? openat(dfd, ent->d_name, OPEN_DIR_FLAGS) : -1;
			if (child >= 0) {
				char sub[PATH_MAX + NAME_MAX + 2];
				snprintf(sub, sizeof sub, "%s%s%s", dir, *dir ? "/" : "", ent->d_name);
				walk(child, h, sub, depth + 1);
				close(child);
			}
		}
}

static const char *relative_to(const char *point, const char *base)
{
	return strcmp(base, "/") ? point + strlen(base) + 1 : point + 1;
}

static void measure_covered(int tree, const char *rel, struct hidden_dir *h)
{
	struct stat st;
	if (fstatat(tree, rel, &st, AT_SYMLINK_NOFOLLOW)) {
		h->err = errno;
		return;
	}
	if (!S_ISDIR(st.st_mode)) {
		h->size = (uint64_t)st.st_blocks * 512;
		h->files = 1;
		return;
	}
	int dfd = openat(tree, rel, OPEN_DIR_FLAGS);
	if (dfd < 0) {
		h->err = errno;
		return;
	}
	walk(dfd, h, "", 0);
	close(dfd);
}

static void add_hidden(struct explain *e, struct hidden_dir h)
{
	if (e->nhidden == e->caphidden)
		e->hidden = grow(e->hidden, &e->caphidden, sizeof *e->hidden);
	e->hidden[e->nhidden++] = h;
}

static void collect_hidden_in(struct explain *e, int index)
{
	const struct mount *m = mounts_get(index);
	int tree = -1;
	for (int i = 0; i < mounts_count(); i++) {
		const struct mount *x = mounts_get(i);
		if (i == index || x->parent != m->id || !strcmp(x->point, m->point))
			continue;
		if (tree < 0 && (tree = (int)open_tree(AT_FDCWD, m->point, OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC)) < 0) {
			add_hidden(e, (struct hidden_dir){ .mount = index, .covered_by = -1, .err = errno });
			return;
		}
		struct hidden_dir h = { .mount = index, .covered_by = i };
		measure_covered(tree, relative_to(x->point, m->point), &h);
		if (h.size || h.err)
			add_hidden(e, h);
	}
	if (tree >= 0)
		close(tree);
}

void explain_collect(struct explain *e, const int *mounts, size_t nmounts)
{
	collect_held(e);
	for (size_t i = 0; i < nmounts; i++)
		if (mounts_get(mounts[i]))
			collect_hidden_in(e, mounts[i]);
}

uint64_t explain_held(const struct explain *e, uint64_t dev, size_t *count)
{
	uint64_t total = 0;
	*count = 0;
	for (size_t i = 0; i < e->nheld; i++)
		if (e->held[i].dev == dev) {
			total += e->held[i].size;
			(*count)++;
		}
	return total;
}

void explain_free(struct explain *e)
{
	for (size_t i = 0; i < e->nheld; i++)
		free(e->held[i].path);
	for (size_t i = 0; i < e->nhidden; i++)
		free(e->hidden[i].largest);
	free(e->held);
	free(e->hidden);
	memset(e, 0, sizeof *e);
}

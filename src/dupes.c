#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "longpath.h"
#include "config.h"
#include "fmt.h"

#include "dupes.h"

#define HEAD_BYTES (64u << 10)
#define READ_CHUNK (1u << 20)

struct candidate {
	const struct node *node;
	const struct node *dir;
	uint64_t length;
	uint64_t head[2];
	uint64_t full[2];
	bool failed;
};

struct pass {
	struct candidate *items;
	size_t count;
	bool full;
	atomic_size_t next;
	atomic_size_t done;
	atomic_uint_fast64_t bytes;
};

static void *grow_or_die(void *p, size_t size)
{
	p = realloc(p, size);
	if (!p) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	return p;
}

static uint64_t mix(uint64_t x)
{
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccdull;
	x ^= x >> 33;
	x *= 0xc4ceb9fe1a85ec53ull;
	return x ^ (x >> 33);
}

static void hash_block(uint64_t h[2], const unsigned char *p, size_t len)
{
	size_t i = 0;
	for (; i + 8 <= len; i += 8) {
		uint64_t w;
		memcpy(&w, p + i, 8);
		h[0] = (h[0] ^ mix(w)) * 0x9e3779b97f4a7c15ull + 0x632be59bd9b4e019ull;
		h[1] = (h[1] + w) * 0xbf58476d1ce4e5b9ull ^ (h[1] >> 29);
	}
	uint64_t tail = 0;
	memcpy(&tail, p + i, len - i);
	h[0] = mix(h[0] ^ tail ^ len);
	h[1] = mix(h[1] + tail + (len << 1));
}

static void hash_file(struct candidate *c, bool full, unsigned char *buf, atomic_uint_fast64_t *bytes)
{
	char path[LONG_PATH];
	node_path_in(c->dir, c->node, path, sizeof path);
	int fd = open_long(path, O_RDONLY | O_NOFOLLOW | O_CLOEXEC | O_NOATIME);
	if (fd < 0)
		fd = open_long(path, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
	struct stat st;
	if (fd < 0 || fstat(fd, &st) || !S_ISREG(st.st_mode)) {
		c->failed = true;
		if (fd >= 0)
			close(fd);
		return;
	}
	uint64_t *h = full ? c->full : c->head;
	h[0] = 0x84222325cbf29ce4ull;
	h[1] = (uint64_t)st.st_size;
	c->length = (uint64_t)st.st_size;
	uint64_t limit = full ? UINT64_MAX : HEAD_BYTES;
	ssize_t got;
	for (uint64_t done = 0; done < limit && (got = read(fd, buf, READ_CHUNK)) > 0; done += (uint64_t)got) {
		hash_block(h, buf, (size_t)got);
		atomic_fetch_add_explicit(bytes, (uint64_t)got, memory_order_relaxed);
	}
	close(fd);
}

static void *pass_worker(void *arg)
{
	struct pass *p = arg;
	unsigned char *buf = malloc(READ_CHUNK);
	if (!buf)
		return NULL;
	for (size_t i; (i = atomic_fetch_add(&p->next, 1)) < p->count;) {
		hash_file(&p->items[i], p->full, buf, &p->bytes);
		atomic_fetch_add(&p->done, 1);
	}
	free(buf);
	return NULL;
}

static void show_progress(struct pass *p, bool final)
{
	char read[FMT_LEN];
	fmt_size(atomic_load(&p->bytes), read);
	fprintf(stderr, "\r\033[Kduplicates: %s %zu/%zu files, %s read%s", p->full ? "comparing" : "sampling",
		atomic_load(&p->done), p->count, read, final ? "\n" : "");
}

static uint64_t run_pass(struct candidate *items, size_t count, bool full, int jobs, bool progress)
{
	struct pass p = { .items = items, .count = count, .full = full };
	pthread_t threads[64];
	int n = jobs < 1 ? 1 : jobs > 64 ? 64 : jobs;
	struct timespec tick = { 0, PROGRESS_INTERVAL_MS * 1000000L };
	for (int i = 0; i < n; i++)
		pthread_create(&threads[i], NULL, pass_worker, &p);
	while (progress && atomic_load(&p.done) < count) {
		show_progress(&p, false);
		nanosleep(&tick, NULL);
	}
	if (progress)
		show_progress(&p, true);
	for (int i = 0; i < n; i++)
		pthread_join(threads[i], NULL);
	return atomic_load(&p.bytes);
}

struct gather {
	struct candidate *items;
	size_t count;
	size_t cap;
	uint64_t min;
};

static void gather_files(struct gather *g, const struct node *dir)
{
	for (const struct node *c = node_child(dir); c; c = node_next(c)) {
		if (c->flags & NODE_DIR) {
			gather_files(g, c);
			continue;
		}
		if (node_size(c) < g->min || c->flags & (NODE_LINK | NODE_HARDLINK | NODE_ERROR))
			continue;
		if (g->count == g->cap) {
			g->cap = g->cap ? g->cap * 2 : 1024;
			g->items = grow_or_die(g->items, g->cap * sizeof *g->items);
		}
		g->items[g->count++] = (struct candidate){ .node = c, .dir = dir };
	}
}

static int by_size(const void *a, const void *b)
{
	const struct candidate *x = a, *y = b;
	return node_size(x->node) == node_size(y->node) ? 0 : node_size(x->node) < node_size(y->node) ? 1 : -1;
}

static int by_head(const void *a, const void *b)
{
	const struct candidate *x = a, *y = b;
	if (x->failed != y->failed)
		return x->failed - y->failed;
	if (x->length != y->length)
		return x->length < y->length ? 1 : -1;
	if (x->head[0] != y->head[0])
		return x->head[0] < y->head[0] ? -1 : 1;
	return x->head[1] < y->head[1] ? -1 : x->head[1] > y->head[1];
}

static int by_full(const void *a, const void *b)
{
	int head = by_head(a, b);
	const struct candidate *x = a, *y = b;
	if (head)
		return head;
	if (x->full[0] != y->full[0])
		return x->full[0] < y->full[0] ? -1 : 1;
	return x->full[1] < y->full[1] ? -1 : x->full[1] > y->full[1];
}

typedef bool (*same_fn)(const struct candidate *, const struct candidate *);

static bool same_size(const struct candidate *a, const struct candidate *b)
{
	return node_size(a->node) == node_size(b->node);
}

static bool same_head(const struct candidate *a, const struct candidate *b)
{
	return !a->failed && !b->failed && !by_head(a, b);
}

static bool same_full(const struct candidate *a, const struct candidate *b)
{
	return same_head(a, b) && !by_full(a, b);
}

static size_t keep_groups(struct candidate *items, size_t count, same_fn same)
{
	size_t out = 0;
	for (size_t start = 0, end; start < count; start = end) {
		for (end = start + 1; end < count && same(&items[start], &items[end]);)
			end++;
		if (end - start < 2)
			continue;
		memmove(items + out, items + start, (end - start) * sizeof *items);
		out += end - start;
	}
	return out;
}

static void add_group(struct dupes *d, const struct candidate *items, size_t count)
{
	d->groups = grow_or_die(d->groups, (d->ngroups + 1) * sizeof *d->groups);
	struct dupe_group *g = &d->groups[d->ngroups++];
	g->size = node_size(items[0].node);
	g->count = count;
	g->files = grow_or_die(NULL, count * sizeof *g->files);
	g->dirs = grow_or_die(NULL, count * sizeof *g->dirs);
	for (size_t i = 0; i < count; i++) {
		g->files[i] = items[i].node;
		g->dirs[i] = items[i].dir;
	}
	d->wasted += g->size * (count - 1);
}

static int by_waste(const void *a, const void *b)
{
	const struct dupe_group *x = a, *y = b;
	uint64_t wx = x->size * (x->count - 1), wy = y->size * (y->count - 1);
	return wx == wy ? 0 : wx < wy ? 1 : -1;
}

void dupes_find(const struct node *root, uint64_t min_size, int jobs, bool progress, struct dupes *d)
{
	struct gather g = { .min = min_size ? min_size : 1 };
	memset(d, 0, sizeof *d);
	gather_files(&g, root);
	if (g.count > 1)
		qsort(g.items, g.count, sizeof *g.items, by_size);
	size_t n = keep_groups(g.items, g.count, same_size);
	d->bytes_read += run_pass(g.items, n, false, jobs, progress);
	qsort(g.items, n, sizeof *g.items, by_head);
	n = keep_groups(g.items, n, same_head);
	d->files_read = n;
	d->bytes_read += run_pass(g.items, n, true, jobs, progress);
	qsort(g.items, n, sizeof *g.items, by_full);
	n = keep_groups(g.items, n, same_full);
	for (size_t start = 0, end; start < n; start = end) {
		for (end = start + 1; end < n && same_full(&g.items[start], &g.items[end]);)
			end++;
		add_group(d, g.items + start, end - start);
	}
	if (d->ngroups > 1)
		qsort(d->groups, d->ngroups, sizeof *d->groups, by_waste);
	free(g.items);
}

void dupes_free(struct dupes *d)
{
	for (size_t i = 0; i < d->ngroups; i++) {
		free(d->groups[i].files);
		free(d->groups[i].dirs);
	}
	free(d->groups);
	memset(d, 0, sizeof *d);
}

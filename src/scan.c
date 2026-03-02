#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <unistd.h>

#include "longpath.h"
#include "config.h"
#include "device.h"
#include "fmt.h"
#include "links.h"
#include "mounts.h"
#include "queue.h"
#include "scan.h"

#define OPEN_DIR_FLAGS (O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC)

struct scanner {
	const struct options *opts;
	struct queue queue;
	pthread_mutex_t claim_lock;
	uint64_t *claimed;
	size_t nclaimed;
	size_t capclaimed;
};

struct worker {
	alignas(64) atomic_uint_fast64_t files;
	atomic_uint_fast64_t dirs;
	pthread_t thread;
	struct scanner *scanner;
	struct arena arena;
	struct scan_error *errors;
	size_t nerrors;
	size_t caperrors;
	struct task *stack;
	size_t nstack;
	size_t capstack;
	struct link_ref *links;
	size_t nlinks;
	size_t caplinks;
	char *dents;
};

static void *grow_array(void *items, size_t *cap, size_t size)
{
	*cap = *cap ? *cap * 2 : 64;
	items = realloc(items, *cap * size);
	if (!items) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	return items;
}

static void record_error(struct worker *w, const char *path, int err)
{
	if (w->nerrors == w->caperrors)
		w->errors = grow_array(w->errors, &w->caperrors, sizeof *w->errors);
	char *copy = strdup(path);
	if (!copy) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	w->errors[w->nerrors++] = (struct scan_error){ copy, err };
}

static void record_node_error(struct worker *w, struct node *n, int err)
{
	char path[LONG_PATH];
	n->flags |= NODE_ERROR;
	node_path(n, path, sizeof path);
	record_error(w, path, err);
}

static void record_child_error(struct worker *w, const struct node *dir, struct node *n, int err)
{
	char path[LONG_PATH];
	n->flags |= NODE_ERROR;
	node_path_in(dir, n, path, sizeof path);
	record_error(w, path, err);
}

static bool should_skip(const struct options *o, int mount)
{
	const struct mount *m = mounts_get(mount);
	if (!m)
		return false;
	if (o->one_fs)
		return true;
	return !o->all_fs && !m->real;
}

static bool claim_device(struct scanner *s, uint64_t dev)
{
	bool fresh = true;
	for (size_t i = 0; i < s->nclaimed && fresh; i++)
		fresh = s->claimed[i] != dev;
	if (fresh) {
		if (s->nclaimed == s->capclaimed)
			s->claimed = grow_array(s->claimed, &s->capclaimed, sizeof *s->claimed);
		s->claimed[s->nclaimed++] = dev;
	}
	return fresh;
}

static bool partial_copy(int mount)
{
	const struct mount *m = mounts_get(mount);
	const struct mount *full = mounts_get(m ? mounts_find(m->dev) : -1);
	return m && m->bind && full && !full->bind;
}

static uint16_t crossing_flag(struct scanner *s, int mount, uint64_t dev)
{
	if (should_skip(s->opts, mount))
		return NODE_SKIPPED;
	if (partial_copy(mount) || !claim_device(s, dev))
		return NODE_SKIPPED | NODE_DUPLICATE;
	return NODE_MOUNT;
}

static int mount_id_of(int dirfd, const char *name)
{
	struct statx sx;
	if (statx(dirfd, name, AT_SYMLINK_NOFOLLOW | AT_NO_AUTOMOUNT, STATX_MNT_ID, &sx) || !(sx.stx_mask & STATX_MNT_ID))
		return -1;
	return (int)sx.stx_mnt_id;
}

static int containing_mount(const struct node *n)
{
	for (const struct node *p = node_parent(n); p; p = node_parent(p))
		if (node_mount(p) >= 0)
			return node_mount(p);
	return -1;
}

static int find_mount(const char *path, int id, uint64_t dev)
{
	int found = mounts_find_point(path);
	if (found < 0 && id >= 0)
		found = mounts_find_id(id);
	return found >= 0 ? found : mounts_find(dev);
}

static void cross_device(struct worker *w, struct node *n, uint64_t dev, int id)
{
	char path[LONG_PATH];
	node_path(n, path, sizeof path);
	const struct mount *outer = mounts_get(containing_mount(n));
	if (outer && id >= 0 && id == outer->id && mounts_find_point(path) < 0) {
		node_dir(n)->mount = (int16_t)mounts_add_subvolume(containing_mount(n), dev, path);
		return;
	}
	int mount = find_mount(path, id, dev);
	node_dir(n)->mount = (int16_t)mount;
	n->flags |= crossing_flag(w->scanner, mount, dev);
}

static void push_task(struct worker *w, struct node *n, uint64_t dev)
{
	if (w->nstack == w->capstack)
		w->stack = grow_array(w->stack, &w->capstack, sizeof *w->stack);
	w->stack[w->nstack++] = (struct task){ n, dev };
}

static void add_dir(struct worker *w, uint64_t parent_dev, struct node *n, const struct stat *st, int fd)
{
	atomic_store_explicit(&w->dirs, atomic_load_explicit(&w->dirs, memory_order_relaxed) + 1, memory_order_relaxed);
	if ((uint64_t)st->st_dev != parent_dev) {
		int id = mount_id_of(fd, n->name);
		pthread_mutex_lock(&w->scanner->claim_lock);
		cross_device(w, n, st->st_dev, id);
		pthread_mutex_unlock(&w->scanner->claim_lock);
	}
	if (!(n->flags & NODE_SKIPPED))
		push_task(w, n, st->st_dev);
}

static void add_file(struct worker *w, const struct node *dir, struct node *n, const struct stat *st)
{
	atomic_store_explicit(&w->files, atomic_load_explicit(&w->files, memory_order_relaxed) + 1, memory_order_relaxed);
	if (S_ISLNK(st->st_mode))
		n->flags |= NODE_LINK;
	if (st->st_nlink < 2)
		return;
	if (w->nlinks == w->caplinks)
		w->links = grow_array(w->links, &w->caplinks, sizeof *w->links);
	w->links[w->nlinks++] = (struct link_ref){ st->st_ino, node_ref_of(dir), node_ref_of(n) };
}

static void add_entry(struct worker *w, struct node *dir, uint64_t dev, int fd, const char *name)
{
	struct stat st;
	bool ok = fstatat(fd, name, &st, AT_SYMLINK_NOFOLLOW) == 0;
	int err = errno;
	uint64_t size = !ok ? 0 : w->scanner->opts->apparent ? (uint64_t)st.st_size : (uint64_t)st.st_blocks * 512;
	struct node *n = node_new(&w->arena, dir, name, ok && S_ISDIR(st.st_mode), size);
	node_push_child(dir, n);
	if (!ok) {
		record_child_error(w, dir, n, err);
		return;
	}
	n->mtime = st.st_mtime > 0 ? (uint32_t)st.st_mtime : 0;
	if (n->flags & NODE_DIR)
		add_dir(w, dev, n, &st, fd);
	else
		add_file(w, dir, n, &st);
}

static bool is_dot(const char *name)
{
	return name[0] == '.' && (!name[1] || (name[1] == '.' && !name[2]));
}

static void read_entries(struct worker *w, struct node *dir, uint64_t dev, int fd)
{
	ssize_t got;
	while ((got = getdents64(fd, w->dents, DENTS_BUFFER)) > 0)
		for (ssize_t off = 0; off < got;) {
			struct dirent64 *e = (struct dirent64 *)(w->dents + off);
			off += e->d_reclen;
			if (!is_dot(e->d_name))
				add_entry(w, dir, dev, fd, e->d_name);
		}
	if (got < 0)
		record_node_error(w, dir, errno);
}

static void scan_open(struct worker *w, struct node *dir, uint64_t dev, int fd, int depth);

static void descend(struct worker *w, int fd, size_t from, int depth)
{
	size_t to = w->nstack;
	for (size_t i = from; i < to; i++) {
		struct task t = w->stack[i];
		if (depth + 1 >= FD_DEPTH_MAX || queue_hungry(&w->scanner->queue)) {
			queue_push(&w->scanner->queue, t);
			continue;
		}
		int child = openat(fd, t.dir->name, OPEN_DIR_FLAGS);
		if (child < 0)
			record_node_error(w, t.dir, errno);
		else
			scan_open(w, t.dir, t.dev, child, depth + 1);
	}
	w->nstack = from;
}

static void scan_open(struct worker *w, struct node *dir, uint64_t dev, int fd, int depth)
{
	size_t from = w->nstack;
	read_entries(w, dir, dev, fd);
	descend(w, fd, from, depth);
	close(fd);
}

static void run_task(struct worker *w, struct task t)
{
	char path[LONG_PATH];
	node_path(t.dir, path, sizeof path);
	int fd = open_long(path, OPEN_DIR_FLAGS);
	if (fd < 0)
		record_node_error(w, t.dir, errno);
	else
		scan_open(w, t.dir, t.dev, fd, 0);
}

static void *worker_main(void *arg)
{
	struct worker *w = arg;
	struct queue *q = &w->scanner->queue;
	struct task t;
	w->dents = malloc(DENTS_BUFFER);
	if (!w->dents) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	while (queue_pop(q, &t)) {
		run_task(w, t);
		queue_finish(q);
	}
	free(w->dents);
	return NULL;
}

static void count_progress(struct worker *workers, int n, uint64_t *files, uint64_t *dirs)
{
	*files = *dirs = 0;
	for (int i = 0; i < n; i++) {
		*files += atomic_load_explicit(&workers[i].files, memory_order_relaxed);
		*dirs += atomic_load_explicit(&workers[i].dirs, memory_order_relaxed);
	}
}

static void show_progress(struct worker *workers, int n, bool final)
{
	uint64_t f, d;
	char files[FMT_LEN], dirs[FMT_LEN];
	count_progress(workers, n, &f, &d);
	fmt_count(f, files);
	fmt_count(d, dirs);
	fprintf(stderr, "\r\033[Kscanning  %s files  %s dirs%s", files, dirs, final ? "\n" : "");
}

static void wait_workers(struct scanner *s, struct worker *workers, int n, bool progress)
{
	while (!queue_wait_idle(&s->queue, PROGRESS_INTERVAL_MS))
		if (progress)
			show_progress(workers, n, false);
	if (progress)
		show_progress(workers, n, true);
}

static int resolve_jobs(int requested, uint64_t dev)
{
	if (requested > 0)
		return requested;
	if (device_rotational(dev))
		return ROTATIONAL_JOBS;
	long cpus = sysconf(_SC_NPROCESSORS_ONLN);
	return cpus < 1 ? 1 : cpus > MAX_AUTO_JOBS ? MAX_AUTO_JOBS : (int)cpus;
}

static struct node *make_root(struct arena *a, const struct options *o, uint64_t *dev)
{
	const char *path = o->root;
	struct stat st;
	if (stat(path, &st) < 0) {
		fprintf(stderr, "diskmoon: %s: %s\n", path, strerror(errno));
		return NULL;
	}
	if (!S_ISDIR(st.st_mode)) {
		fprintf(stderr, "diskmoon: %s: not a directory\n", path);
		return NULL;
	}
	struct node *root = node_new(a, NULL, path, true, o->apparent ? (uint64_t)st.st_size : (uint64_t)st.st_blocks * 512);
	root->mtime = st.st_mtime > 0 ? (uint32_t)st.st_mtime : 0;
	int found = mounts_find_point(path);
	int id = mount_id_of(AT_FDCWD, path);
	if (found < 0 && id >= 0)
		found = mounts_find_id(id);
	node_dir(root)->mount = (int16_t)(found >= 0 ? found : mounts_find(st.st_dev));
	*dev = st.st_dev;
	return root;
}

static void settle_links(struct worker *workers, int n)
{
	size_t total = 0;
	int biggest = 0;
	for (int i = 0; i < n; i++) {
		total += workers[i].nlinks;
		if (workers[i].nlinks > workers[biggest].nlinks)
			biggest = i;
	}
	struct worker *base = &workers[biggest];
	struct link_ref *all = total ? realloc(base->links, total * sizeof *all) : base->links;
	if (!all && total) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	size_t at = base->nlinks;
	for (int i = 0; i < n; i++) {
		if (i == biggest)
			continue;
		if (workers[i].nlinks)
			memcpy(all + at, workers[i].links, workers[i].nlinks * sizeof *all);
		at += workers[i].nlinks;
		free(workers[i].links);
	}
	links_settle(all, total);
	free(all);
}

static int by_path(const void *a, const void *b)
{
	return strcmp(((const struct scan_error *)a)->path, ((const struct scan_error *)b)->path);
}

static void collect(struct worker *workers, int n, struct scan_result *r)
{
	settle_links(workers, n);
	size_t total = 0;
	for (int i = 0; i < n; i++)
		total += workers[i].nerrors;
	r->errors = total ? malloc(total * sizeof *r->errors) : NULL;
	count_progress(workers, n, &r->files, &r->dirs);
	r->dirs++;
	for (int i = 0; i < n; i++) {
		if (workers[i].nerrors)
			memcpy(r->errors + r->nerrors, workers[i].errors, workers[i].nerrors * sizeof *r->errors);
		r->nerrors += r->errors ? workers[i].nerrors : 0;
		free(workers[i].errors);
		free(workers[i].stack);
	}
	if (r->nerrors > 1)
		qsort(r->errors, r->nerrors, sizeof *r->errors, by_path);
}

static double elapsed_since(const struct timespec *start)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (double)(now.tv_sec - start->tv_sec) + (double)(now.tv_nsec - start->tv_nsec) / 1e9;
}

static void start_workers(struct scanner *s, struct worker *workers, int n)
{
	for (int i = 0; i < n; i++) {
		workers[i].scanner = s;
		pthread_create(&workers[i].thread, NULL, worker_main, &workers[i]);
	}
}

int scan_run(const struct options *o, struct scan_result *r)
{
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	memset(r, 0, sizeof *r);
	r->started = (int64_t)time(NULL);
	if (!node_space_init()) {
		fputs("diskmoon: cannot reserve memory for the tree\n", stderr);
		return -1;
	}
	mounts_load();
	struct scanner s = { .opts = o };
	queue_init(&s.queue);
	pthread_mutex_init(&s.claim_lock, NULL);
	struct arena root_arena = { 0 };
	uint64_t dev;
	if (!(r->root = make_root(&root_arena, o, &dev)))
		return -1;
	const struct mount *home = mounts_get(node_mount(r->root));
	r->jobs = resolve_jobs(o->jobs, home ? home->block : dev);
	struct worker *workers = aligned_alloc(64, ((size_t)r->jobs * sizeof *workers + 63) & ~(size_t)63);
	if (!workers)
		return -1;
	memset(workers, 0, (size_t)r->jobs * sizeof *workers);
	claim_device(&s, dev);
	queue_push(&s.queue, (struct task){ r->root, dev });
	start_workers(&s, workers, r->jobs);
	wait_workers(&s, workers, r->jobs, !o->quiet && isatty(STDERR_FILENO));
	for (int i = 0; i < r->jobs; i++)
		pthread_join(workers[i].thread, NULL);
	collect(workers, r->jobs, r);
	free(workers);
	free(s.claimed);
	free(s.queue.items);
	r->seconds = elapsed_since(&start);
	return 0;
}

void scan_result_free(struct scan_result *r)
{
	for (size_t i = 0; i < r->nerrors; i++)
		free(r->errors[i].path);
	free(r->errors);
	r->errors = NULL;
	r->nerrors = 0;
}

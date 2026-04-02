#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fmt.h"
#include "tui_internal.h"

static const char *const sort_names[] = { "size", "name", "modified", "items" };
static const char *const eighths[] = { "", "▏", "▎", "▍", "▌", "▋", "▊", "▉" };

#define BAR_CELLS 10
#define BUILD_WAIT_MS 15

static uint64_t items_of(const struct node *n)
{
	return n->flags & NODE_DIR ? node_dir(n)->items : 1;
}

static int by_name(const void *a, const void *b)
{
	return strcmp((*(struct node *const *)a)->name, (*(struct node *const *)b)->name);
}

static int by_modified(const void *a, const void *b)
{
	const struct node *x = *(struct node *const *)a, *y = *(struct node *const *)b;
	return x->mtime == y->mtime ? by_name(a, b) : x->mtime < y->mtime ? 1 : -1;
}

static int by_items(const void *a, const void *b)
{
	uint64_t x = items_of(*(struct node *const *)a), y = items_of(*(struct node *const *)b);
	return x == y ? by_name(a, b) : x < y ? 1 : -1;
}

enum job_state { JOB_RUNNING, JOB_DONE, JOB_CANCELLED };

struct build_job {
	const struct node *dir;
	const struct node *select;
	bool keep_cursor;
	enum tui_sort sort;
	char filter[TUI_INPUT_MAX];
	const struct node **rows;
	size_t nrows;
	size_t cap;
	bool up_row;
	atomic_int state;
};

static bool push_row(struct build_job *j, const struct node *n)
{
	if (j->nrows == j->cap) {
		size_t cap = j->cap ? j->cap * 2 : 256;
		const struct node **grown = realloc(j->rows, cap * sizeof *grown);
		if (!grown)
			return false;
		j->rows = grown;
		j->cap = cap;
	}
	j->rows[j->nrows++] = n;
	return true;
}

static bool cancelled(struct build_job *j)
{
	return atomic_load_explicit(&j->state, memory_order_relaxed) == JOB_CANCELLED;
}

static void collect_rows(struct build_job *j)
{
	size_t seen = 0;
	j->up_row = node_parent(j->dir) != NULL;
	if (j->up_row && !push_row(j, node_parent(j->dir)))
		return;
	for (const struct node *c = node_child(j->dir); c; c = node_next(c)) {
		if (!(++seen & 8191) && cancelled(j))
			return;
		if ((!j->filter[0] || strcasestr(c->name, j->filter)) && !push_row(j, c))
			return;
	}
}

static void job_free(struct build_job *j)
{
	free(j->rows);
	free(j);
}

static void *build_worker(void *arg)
{
	static int (*const compare[])(const void *, const void *) = { NULL, by_name, by_modified, by_items };
	struct build_job *j = arg;
	collect_rows(j);
	size_t first = j->up_row;
	if (!cancelled(j) && compare[j->sort] && j->nrows > first + 1)
		qsort(j->rows + first, j->nrows - first, sizeof *j->rows, compare[j->sort]);
	if (atomic_exchange(&j->state, JOB_DONE) == JOB_CANCELLED)
		job_free(j);
	return NULL;
}

static void job_cancel(struct build_job *j)
{
	if (j && atomic_exchange(&j->state, JOB_CANCELLED) == JOB_DONE)
		job_free(j);
}

static void select_node(struct files *f, const struct node *n)
{
	for (size_t i = f->up_row; i < f->nrows; i++)
		if (f->rows[i] == n) {
			f->cursor = i;
			size_t page = (size_t)tui_body_height() - 1;
			f->top = i > page / 2 ? i - page / 2 : 0;
			return;
		}
}

static void apply_job(struct files *f, struct build_job *j)
{
	const struct node *keep = j->keep_cursor && f->nrows && f->cursor < f->nrows ? f->rows[f->cursor] : NULL;
	free(f->rows);
	f->rows = j->rows;
	f->nrows = j->nrows;
	f->cap = j->cap;
	f->up_row = j->up_row;
	f->dir = j->dir;
	f->sort = j->sort;
	memcpy(f->filter, j->filter, sizeof f->filter);
	f->cursor = f->up_row && f->nrows > 1;
	f->top = 0;
	if (j->select || keep)
		select_node(f, j->select ? j->select : keep);
	free(j);
	f->job = NULL;
}

static bool job_done(const struct build_job *j)
{
	return atomic_load(&((struct build_job *)j)->state) == JOB_DONE;
}

static void wait_briefly(struct files *f)
{
	struct timespec tick = { 0, 1000000L };
	for (int i = 0; i < BUILD_WAIT_MS && !job_done(f->job); i++)
		nanosleep(&tick, NULL);
	if (job_done(f->job))
		apply_job(f, f->job);
}

static void start_build(struct tui *t, const struct node *dir, const struct node *select, enum tui_sort sort, const char *filter)
{
	struct files *f = &t->files;
	job_cancel(f->job);
	f->job = NULL;
	struct build_job *j = calloc(1, sizeof *j);
	if (!j)
		return;
	*j = (struct build_job){ .dir = dir, .select = select, .keep_cursor = !select && dir == f->dir, .sort = sort };
	snprintf(j->filter, sizeof j->filter, "%s", filter);
	atomic_init(&j->state, JOB_RUNNING);
	f->job = j;
	pthread_attr_t attr;
	pthread_t thread;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&thread, &attr, build_worker, j))
		build_worker(j);
	pthread_attr_destroy(&attr);
	wait_briefly(f);
}

bool files_busy(const struct tui *t)
{
	return t->files.job != NULL;
}

void files_poll(struct tui *t)
{
	if (t->files.job && job_done(t->files.job))
		apply_job(&t->files, t->files.job);
}

static enum tui_sort pending_sort(const struct files *f)
{
	return f->job ? f->job->sort : f->sort;
}

static const struct node *pending_dir(const struct files *f)
{
	return f->job ? f->job->dir : f->dir;
}

void files_open(struct tui *t, const struct node *dir, const struct node *select)
{
	start_build(t, dir, select, pending_sort(&t->files), "");
}

void files_reveal(struct tui *t, const struct node *n)
{
	const struct node *parent = node_parent(n);
	files_open(t, parent ? parent : n, parent ? n : NULL);
	t->view = VIEW_FILES;
}

const struct node *files_current(const struct tui *t)
{
	const struct files *f = &t->files;
	if (!f->nrows || (f->up_row && !f->cursor))
		return NULL;
	return f->rows[f->cursor];
}

static void draw_bar(int x, int y, uint64_t part, uint64_t whole, bool ascii, struct term_style style)
{
	unsigned units = whole ? (unsigned)((long double)part / whole * BAR_CELLS * 8 + 0.5L) : 0;
	struct term_style bar = { style.attr & ~TERM_REVERSE, style.fg ? style.fg : TERM_CYAN, 0 };
	for (int i = 0; i < BAR_CELLS; i++, units = units > 8 ? units - 8 : 0) {
		const char *piece = units >= 8 ? (ascii ? "#" : "█") : ascii ? (units >= 4 ? "#" : " ") : eighths[units];
		term_put(x + i, y, 1, *piece && *piece != ' ' ? bar : style, *piece ? piece : " ");
	}
}

static int put_tag(int x, int y, const char *tag, struct term_style style)
{
	if (x >= term_width())
		return x;
	return x + term_put(x, y, term_width() - x, style, tag);
}

static void draw_name(struct tui *t, const struct node *n, bool up, int x, int y, struct term_style style)
{
	if (up) {
		term_put(x, y, term_width() - x, style, "..  (back)");
		return;
	}
	x += term_put(x, y, term_width() - x, n->flags & NODE_DIR ? (struct term_style){ style.attr | TERM_BOLD, style.fg, style.bg } : style, n->name);
	if (n->flags & NODE_DIR)
		x = put_tag(x, y, "/", style);
	if (n->flags & NODE_LINK)
		x = put_tag(x, y, " -> link", (struct term_style){ style.attr | TERM_DIM, style.fg, style.bg });
	if (n->flags & NODE_MOUNT)
		x = put_tag(x, y, "  [other disk]", (struct term_style){ style.attr, TERM_BLUE, style.bg });
	if (n->flags & NODE_HARDLINK)
		x = put_tag(x, y, "  [hard link, counted elsewhere]", (struct term_style){ style.attr | TERM_DIM, style.fg, style.bg });
	if (n->flags & NODE_ERROR)
		x = put_tag(x, y, "  [could not read]", (struct term_style){ style.attr, TERM_RED, style.bg });
	if (n->flags & NODE_CLAIMED)
		put_tag(x, y, "  [can free, see Free]", (struct term_style){ style.attr, TERM_GREEN, style.bg });
	(void)t;
}

static void draw_row(struct tui *t, size_t i, int y)
{
	const struct files *f = &t->files;
	const struct node *n = f->rows[i];
	bool up = f->up_row && i == 0, wide = term_width() >= 84;
	struct term_style style = i == f->cursor ? tui_select : tui_plain;
	char size[FMT_LEN] = "", pct[FMT_LEN] = "", items[FMT_LEN] = "", date[FMT_LEN] = "", cell[64];
	term_fill(0, y, term_width(), style);
	if (!up) {
		fmt_size(node_size(n), size);
		fmt_percent(node_size(n), node_size(f->dir), pct);
		fmt_count(items_of(n), items);
		fmt_date(n->mtime, date);
	}
	snprintf(cell, sizeof cell, "%9s %6s", size, pct);
	int x = 1 + term_put(1, y, 17, style, cell) + 1;
	if (!up)
		draw_bar(x, y, node_size(n), node_size(f->dir), t->o->ascii, style);
	x += BAR_CELLS + 1;
	if (wide) {
		snprintf(cell, sizeof cell, "%11s  %10s", up ? "" : items, date);
		x += term_put(x, y, 23, style, cell);
	}
	draw_name(t, n, up, x + 2, y, style);
}

void files_draw(struct tui *t, int y, int h)
{
	struct files *f = &t->files;
	char head[128];
	bool wide = term_width() >= 84;
	snprintf(head, sizeof head, "%9s %6s %-10s %s", "SIZE", "%", " SHARE", wide ? "        ITEMS    MODIFIED  NAME" : " NAME");
	term_fill(0, y, term_width(), tui_dim);
	term_put(1, y, term_width() - 1, tui_dim, head);
	snprintf(head, sizeof head, "sorted by %s%s%s%s", sort_names[f->sort], f->filter[0] ? ", filter \"" : "", f->filter, f->filter[0] ? "\"" : "");
	term_put(term_width() - term_text_width(head) - 1, y, term_width(), tui_dim, head);
	size_t rows = (size_t)(h - 1);
	if (f->cursor < f->top)
		f->top = f->cursor;
	if (f->cursor >= f->top + rows)
		f->top = f->cursor - rows + 1;
	for (size_t r = 0; r < rows; r++) {
		term_fill(0, y + 1 + (int)r, term_width(), tui_plain);
		if (f->top + r < f->nrows)
			draw_row(t, f->top + r, y + 1 + (int)r);
	}
	if (f->nrows == f->up_row)
		term_put(4, y + 2 + (int)f->up_row, term_width() - 4, tui_dim, f->filter[0] ? "nothing here matches the filter" : "empty folder");
}

static void go_up(struct tui *t)
{
	const struct node *from = t->files.dir, *parent = node_parent(from);
	if (parent)
		files_open(t, parent, from);
}

static void open_current(struct tui *t)
{
	struct files *f = &t->files;
	if (!f->nrows)
		return;
	if (f->up_row && !f->cursor) {
		go_up(t);
		return;
	}
	const struct node *n = f->rows[f->cursor];
	if (n->flags & NODE_DIR && node_child(n))
		files_open(t, n, NULL);
	else if (n->flags & NODE_DIR)
		tui_status(t, "this folder is empty or was not read");
}

static void on_filter(struct tui *t, bool done)
{
	struct files *f = &t->files;
	const char *current = f->job ? f->job->filter : f->filter;
	if (done && !strcmp(current, t->input))
		return;
	start_build(t, pending_dir(f), NULL, pending_sort(f), t->input);
}

bool files_key(struct tui *t, int key)
{
	struct files *f = &t->files;
	if (tui_move(&f->cursor, &f->top, f->nrows, (size_t)tui_body_height() - 1, key))
		return true;
	switch (key) {
	case TERM_KEY_ENTER: case TERM_KEY_RIGHT: case 'l': open_current(t); return true;
	case TERM_KEY_LEFT: case TERM_KEY_BACKSPACE: case 'h': go_up(t); return true;
	case '/': tui_ask(t, "filter: ", f->filter, on_filter); return true;
	case TERM_KEY_ESCAPE:
		if (f->job) {
			job_cancel(f->job);
			f->job = NULL;
			tui_status(t, "stopped");
		} else if (f->filter[0]) {
			start_build(t, f->dir, NULL, f->sort, "");
		}
		return true;
	case 's':
		start_build(t, pending_dir(f), NULL, (enum tui_sort)((pending_sort(f) + 1) % SORT_COUNT), f->job ? f->job->filter : f->filter);
		tui_status(t, "sorted by %s", sort_names[pending_sort(f)]);
		return true;
	default: return false;
	}
}

void files_close(struct tui *t)
{
	job_cancel(t->files.job);
	free(t->files.rows);
	t->files = (struct files){ 0 };
}

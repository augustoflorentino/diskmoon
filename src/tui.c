#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "diff.h"
#include "fmt.h"
#include "history.h"
#include "import.h"
#include "report.h"
#include "tui_internal.h"

#define BUSY_POLL_MS 15

const struct term_style tui_plain = { 0, 0, 0 };
const struct term_style tui_dim = { TERM_DIM, 0, 0 };
const struct term_style tui_bold = { TERM_BOLD, 0, 0 };
const struct term_style tui_select = { TERM_REVERSE, 0, 0 };
const struct term_style tui_title = { TERM_BOLD | TERM_REVERSE, 0, 0 };
const struct term_style tui_safe = { TERM_BOLD, TERM_GREEN, 0 };
const struct term_style tui_check = { TERM_BOLD, TERM_YELLOW, 0 };
const struct term_style tui_alert = { TERM_BOLD, TERM_RED, 0 };

static const char *const tab_names[] = { "1 Report", "2 Files", "3 Free", "4 Changes" };

static const char help_text[] =
	"KEYS\n"
	"=====\n"
	"  1 2 3 4, tab      switch between Report, Files, Free and Changes\n"
	"  up down, j k      move;  page up/down, home/end for bigger jumps\n"
	"  enter, right      open the folder (in Free: show the item in Files)\n"
	"  left, backspace   back to the parent folder\n"
	"  /                 Files: filter this folder by name;  other tabs: find text (n = next)\n"
	"  esc               clear the filter\n"
	"  s                 Files: sort by size, name, last change or item count\n"
	"  w                 write the full report to a .txt file in the current folder\n"
	"  S                 save this scan, so the Changes tab can compare against it next time\n"
	"  ?                 this help;  q quits\n"
	"\n"
	"WHAT THE TABS SHOW\n"
	"=====\n"
	"  Report   plain-language summary, what can be freed, disks, space nobody can see,\n"
	"           file types, folders untouched for a year, biggest files\n"
	"  Files    every folder and file, biggest first, with share of the parent folder\n"
	"  Free     caches, build output and other things safe to delete, with the command for each\n"
	"  Changes  what grew, shrank, appeared or went away since the last saved scan\n"
	"\n"
	"NOTHING IS DELETED FROM HERE. diskmoon only reads; the commands are for you to run.\n";

int tui_body_height(void)
{
	return term_height() - TUI_TOP - 1;
}

void tui_status(struct tui *t, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(t->status, sizeof t->status, fmt, ap);
	va_end(ap);
}

void tui_ask(struct tui *t, const char *prompt, const char *initial, input_fn on_input)
{
	t->typing = true;
	t->prompt = prompt;
	t->on_input = on_input;
	snprintf(t->input, sizeof t->input, "%s", initial);
	t->inputlen = strlen(t->input);
}

bool tui_move(size_t *cursor, size_t *top, size_t count, size_t page, int key)
{
	size_t last = count ? count - 1 : 0;
	(void)top;
	switch (key) {
	case TERM_KEY_UP: case 'k': *cursor -= *cursor > 0; return true;
	case TERM_KEY_DOWN: case 'j': *cursor += *cursor < last; return true;
	case TERM_KEY_PAGE_UP: *cursor = *cursor > page ? *cursor - page : 0; return true;
	case TERM_KEY_PAGE_DOWN: *cursor = *cursor + page < last ? *cursor + page : last; return true;
	case TERM_KEY_HOME: case 'g': *cursor = 0; return true;
	case TERM_KEY_END: case 'G': *cursor = last; return true;
	default: return false;
	}
}

static void render(FILE **f, char **data, size_t *size)
{
	*data = NULL;
	*size = 0;
	*f = open_memstream(data, size);
}

static void load_report(struct tui *t)
{
	struct options o = *t->o;
	char *data;
	size_t size;
	FILE *f;
	o.max_depth = 1;
	o.min_size = node_size(t->r->root) / 100;
	o.top = 20;
	render(&f, &data, &size);
	if (!f)
		return;
	report_write(f, &o, t->r, t->s, t->e, t->a);
	fclose(f);
	pager_load(&t->report, data, size);
}

static void load_text(struct pager *p, const char *fmt, ...)
{
	char *data;
	size_t size;
	FILE *f;
	render(&f, &data, &size);
	if (!f)
		return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(f, fmt, ap);
	va_end(ap);
	fclose(f);
	pager_load(p, data, size);
}

static void load_changes(struct tui *t)
{
	char path[LONG_PATH];
	const char *root = t->r->root->name;
	t->changes_ready = true;
	if (!history_latest(root, t->saved[0] ? t->saved : NULL, path, sizeof path)) {
		load_text(&t->changes,
			  "No saved scan of %s yet.\n\n"
			  "Press S to save this one. The next time you open diskmoon on this folder,\n"
			  "this tab shows what grew, shrank, appeared and went away since then.\n", root);
		return;
	}
	struct options o = *t->o;
	struct scan_result old;
	struct tree_stats st;
	o.top = 0;
	o.quiet = true;
	if (import_ncdu(&o, path, &old)) {
		load_text(&t->changes, "Could not read the previous scan:\n%s\n", path);
		return;
	}
	tree_finalize(old.root, 0, &st);
	char *data;
	size_t size;
	FILE *f;
	render(&f, &data, &size);
	if (!f)
		return;
	diff_write(f, &o, &old, t->r);
	fclose(f);
	pager_load(&t->changes, data, size);
	tui_status(t, "compared with %s", path);
}

static void write_report(struct tui *t)
{
	char name[LONG_PATH], stamp[32];
	time_t now = time(NULL);
	struct tm tm;
	localtime_r(&now, &tm);
	strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tm);
	snprintf(name, sizeof name, DEFAULT_REPORT_PREFIX "-%s.txt", stamp);
	FILE *f = fopen(name, "w");
	if (!f) {
		tui_status(t, "cannot write %s here; run diskmoon from a folder you can write to", name);
		return;
	}
	setvbuf(f, NULL, _IOFBF, OUTPUT_BUFFER);
	report_write(f, t->o, t->r, t->s, t->e, t->a);
	char *where = realpath(".", NULL);
	tui_status(t, fclose(f) ? "error while writing %s/%s" : "report written to %s/%s", where ? where : ".", name);
	free(where);
}

static void save_scan(struct tui *t)
{
	char path[LONG_PATH] = "";
	if (t->r->source) {
		tui_status(t, "this tree was loaded from %s; it is already saved", t->r->source);
		return;
	}
	if (!history_save(t->o, t->r, path, sizeof path)) {
		tui_status(t, "could not save the scan");
		return;
	}
	snprintf(t->saved, sizeof t->saved, "%s", path);
	tui_status(t, "scan saved; next time the Changes tab compares against it");
}

static void draw_title(struct tui *t)
{
	char path[LONG_PATH], right[160], size[FMT_LEN], items[FMT_LEN];
	const struct node *dir = t->view == VIEW_FILES ? t->files.dir : t->r->root;
	node_path(dir, path, sizeof path);
	fmt_size(node_size(dir), size);
	fmt_count(node_dir(dir)->items, items);
	snprintf(right, sizeof right, "%s in %s items ", size, items);
	term_fill(0, 0, term_width(), tui_title);
	int x = term_put(0, 0, term_width(), tui_title, " ");
	int room = term_width() - x - term_text_width(right) - 1;
	term_put(x, 0, room > 0 ? room : 0, tui_title, path);
	term_put(term_width() - term_text_width(right), 0, term_width(), tui_title, right);
}

static void draw_tabs(struct tui *t)
{
	int x = 1;
	term_fill(0, 1, term_width(), tui_plain);
	for (int i = 0; i < 4; i++) {
		struct term_style s = (int)t->view == i ? tui_select : tui_dim;
		x += term_put(x, 1, term_width() - x, s, " ");
		x += term_put(x, 1, term_width() - x, s, tab_names[i]);
		x += term_put(x, 1, term_width() - x, s, " ") + 1;
	}
	term_put(x + 1, 1, term_width() - x - 1, tui_dim, "?  help   q  quit");
}

static const char *hints(const struct tui *t)
{
	switch (t->view) {
	case VIEW_FILES: return "enter open  left back  / filter  s sort  w write report  S save scan";
	case VIEW_FREE: return "up/down choose  enter show in Files  w write report";
	default: return "up/down scroll  / find  n next  left/right pan  w write report";
	}
}

static void draw_footer(struct tui *t)
{
	int y = term_height() - 1;
	term_fill(0, y, term_width(), tui_dim);
	if (t->typing) {
		int x = 1 + term_put(1, y, term_width() - 1, tui_bold, t->prompt);
		x += term_put(x, y, term_width() - x, tui_plain, t->input);
		term_cursor(x, y);
		return;
	}
	if (files_busy(t)) {
		term_put(1, y, term_width() - 1, tui_bold, "reading the folder...  (esc stops)");
		return;
	}
	if (t->status[0])
		term_put(1, y, term_width() - 1, tui_plain, t->status);
}

static void draw_view(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
	struct tui *t = ctx;
	(void)app;
	term_begin(canvas);
	draw_title(t);
	draw_tabs(t);
	int h = tui_body_height();
	switch (t->view) {
	case VIEW_REPORT: pager_draw(&t->report, TUI_TOP, h); break;
	case VIEW_FILES: files_draw(t, TUI_TOP, h); break;
	case VIEW_FREE: free_draw(t, TUI_TOP, h); break;
	case VIEW_CHANGES: pager_draw(&t->changes, TUI_TOP, h); break;
	case VIEW_HELP: pager_draw(&t->help, TUI_TOP, h); break;
	}
	draw_footer(t);
}

static size_t put_utf8(char *out, unsigned c)
{
	if (c < 0x80)
		return (size_t)(out[0] = (char)c, 1);
	if (c < 0x800) {
		out[0] = (char)(0xc0 | c >> 6);
		out[1] = (char)(0x80 | (c & 0x3f));
		return 2;
	}
	if (c < 0x10000) {
		out[0] = (char)(0xe0 | c >> 12);
		out[1] = (char)(0x80 | (c >> 6 & 0x3f));
		out[2] = (char)(0x80 | (c & 0x3f));
		return 3;
	}
	out[0] = (char)(0xf0 | c >> 18);
	out[1] = (char)(0x80 | (c >> 12 & 0x3f));
	out[2] = (char)(0x80 | (c >> 6 & 0x3f));
	out[3] = (char)(0x80 | (c & 0x3f));
	return 4;
}

static void type_key(struct tui *t, int key)
{
	if (key == TERM_KEY_ENTER || key == TERM_KEY_ESCAPE) {
		if (key == TERM_KEY_ESCAPE)
			t->input[t->inputlen = 0] = '\0';
		t->typing = false;
		t->on_input(t, true);
		return;
	}
	if (key == TERM_KEY_BACKSPACE && t->inputlen) {
		while (t->inputlen && (t->input[--t->inputlen] & 0xc0) == 0x80)
			;
		t->input[t->inputlen] = '\0';
	} else if (key >= ' ' && key < TERM_KEY_UP && t->inputlen + 4 < sizeof t->input) {
		t->inputlen += put_utf8(t->input + t->inputlen, (unsigned)key);
		t->input[t->inputlen] = '\0';
	} else {
		return;
	}
	t->on_input(t, false);
}

static void switch_view(struct tui *t, enum tui_view v)
{
	t->view = v;
	if (v == VIEW_CHANGES && !t->changes_ready)
		load_changes(t);
}

static bool global_key(struct tui *t, int key)
{
	switch (key) {
	case 'q': pnl_app_quit(t->app); return true;
	case '1': case '2': case '3': case '4': switch_view(t, (enum tui_view)(key - '1')); return true;
	case TERM_KEY_TAB: switch_view(t, (enum tui_view)((t->view + 1) % 4)); return true;
	case TERM_KEY_BACKTAB: switch_view(t, (enum tui_view)((t->view + 3) % 4)); return true;
	case 'w': write_report(t); return true;
	case 'S': save_scan(t); return true;
	case '?':
		if (t->view != VIEW_HELP)
			t->before_help = t->view;
		t->view = t->view == VIEW_HELP ? t->before_help : VIEW_HELP;
		return true;
	case TERM_CTRL('l'): pnl_app_redraw(t->app); return true;
	default: return false;
	}
}

static void view_key(struct tui *t, int key)
{
	bool used = false;
	switch (t->view) {
	case VIEW_REPORT: used = pager_key(t, &t->report, key); break;
	case VIEW_FILES: used = files_key(t, key); break;
	case VIEW_FREE: used = free_key(t, key); break;
	case VIEW_CHANGES: used = pager_key(t, &t->changes, key); break;
	case VIEW_HELP: used = key == TERM_KEY_ESCAPE ? (t->view = t->before_help, true) : pager_key(t, &t->help, key); break;
	}
	if (!used)
		global_key(t, key);
}

static void handle_key(struct tui *t, int key)
{
	if (key == TERM_KEY_NONE)
		return;
	t->status[0] = '\0';
	if (t->typing)
		type_key(t, key);
	else
		view_key(t, key);
}

static void release(struct tui *t)
{
	pager_free(&t->report);
	pager_free(&t->changes);
	pager_free(&t->help);
	files_close(t);
	free(t->free.order);
}

static void poll_folder(void *ctx, pnl_app_t *app)
{
	struct tui *t = ctx;
	files_poll(t);
	if (!files_busy(t)) {
		pnl_app_remove_timer(app, t->timer);
		t->timer = 0;
	}
	pnl_app_redraw(app);
}

static void after_key(struct tui *t)
{
	if (files_busy(t) && !t->timer)
		pnl_app_add_timer(t->app, BUSY_POLL_MS, poll_folder, t, &t->timer);
	pnl_view_set_hints(t->app, hints(t));
}

static int key_view(void *ctx, pnl_app_t *app, int key)
{
	struct tui *t = ctx;
	(void)app;
	if (key == PNL_KEY_WHEEL_UP || key == PNL_KEY_WHEEL_DOWN)
		return 0;
	if (key != PNL_KEY_CLICK)
		handle_key(t, key);
	after_key(t);
	return 1;
}

static int open_view(struct tui *t)
{
	char name[] = "diskmoon";
	char *argv[] = { name, NULL };
	int status = pnl_view_push(t->app, NULL, draw_view, key_view, t);
	after_key(t);
	return status ? status : pnl_app_run(t->app, 1, argv, NULL);
}

int tui_run(const struct options *o, const struct scan_result *r, const struct tree_stats *s, const struct explain *e,
	    const struct analysis *a)
{
	struct tui t = { .o = o, .r = r, .s = s, .e = e, .a = a };
	if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO) || pnl_app_create("diskmoon", NULL, &t.app)) {
		fputs("diskmoon: the interactive view needs a terminal; use -o FILE for a report\n", stderr);
		return 1;
	}
	load_report(&t);
	load_text(&t.help, "%s", help_text);
	free_init(&t);
	files_open(&t, r->root, NULL);
	tui_status(&t, "? shows every key.  3 = what you can free.  2 = browse folders.");
	int status = open_view(&t);
	release(&t);
	pnl_app_destroy(t.app);
	if (status)
		fprintf(stderr, "diskmoon: %s\n", pnl_strerror(status));
	return status ? 1 : 0;
}

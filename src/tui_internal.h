#ifndef DISKMOON_TUI_INTERNAL_H
#define DISKMOON_TUI_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>

#include "term.h"
#include "tui.h"

#define TUI_INPUT_MAX 256
#define TUI_TOP 2

enum tui_view { VIEW_REPORT, VIEW_FILES, VIEW_FREE, VIEW_CHANGES, VIEW_HELP };
enum tui_sort { SORT_SIZE, SORT_NAME, SORT_MODIFIED, SORT_ITEMS, SORT_COUNT };

struct pager {
	char *data;
	size_t size;
	size_t *lines;
	size_t nlines;
	size_t top;
	size_t left;
	size_t match;
	bool has_match;
	char search[TUI_INPUT_MAX];
};

struct build_job;

struct files {
	struct build_job *job;
	const struct node *dir;
	const struct node **rows;
	size_t nrows;
	size_t cap;
	bool up_row;
	size_t cursor;
	size_t top;
	enum tui_sort sort;
	char filter[TUI_INPUT_MAX];
};

struct free_list {
	size_t *order;
	size_t count;
	size_t cursor;
	size_t top;
};

struct tui;
typedef void (*input_fn)(struct tui *t, bool done);

struct tui {
	const struct options *o;
	const struct scan_result *r;
	const struct tree_stats *s;
	const struct explain *e;
	const struct analysis *a;
	enum tui_view view;
	enum tui_view before_help;
	struct files files;
	struct free_list free;
	struct pager report;
	struct pager changes;
	struct pager help;
	bool changes_ready;
	char saved[4096];
	char status[512];
	bool typing;
	const char *prompt;
	char input[TUI_INPUT_MAX];
	size_t inputlen;
	input_fn on_input;
	pnl_app_t *app;
	int timer;
};

extern const struct term_style tui_plain, tui_dim, tui_bold, tui_select, tui_title, tui_safe, tui_check, tui_alert;

int tui_body_height(void);
void tui_status(struct tui *t, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void tui_ask(struct tui *t, const char *prompt, const char *initial, input_fn on_input);
bool tui_move(size_t *cursor, size_t *top, size_t count, size_t page, int key);

void files_open(struct tui *t, const struct node *dir, const struct node *select);
void files_close(struct tui *t);
void files_reveal(struct tui *t, const struct node *n);
void files_draw(struct tui *t, int y, int h);
bool files_key(struct tui *t, int key);
const struct node *files_current(const struct tui *t);
bool files_busy(const struct tui *t);
void files_poll(struct tui *t);

void free_init(struct tui *t);
void free_draw(struct tui *t, int y, int h);
bool free_key(struct tui *t, int key);

void pager_load(struct pager *p, char *data, size_t size);
void pager_free(struct pager *p);
void pager_draw(struct pager *p, int y, int h);
bool pager_key(struct tui *t, struct pager *p, int key);
void pager_search(struct tui *t, struct pager *p);

#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "tui_internal.h"

#define DETAIL_LINES (FINDING_PATHS + 4)

static const struct analysis *sorting;

static int by_size(const void *a, const void *b)
{
	uint64_t x = sorting->findings[*(const size_t *)a].size, y = sorting->findings[*(const size_t *)b].size;
	return x == y ? 0 : x < y ? 1 : -1;
}

void free_init(struct tui *t)
{
	struct free_list *l = &t->free;
	l->order = malloc((t->a->nfindings + 1) * sizeof *l->order);
	if (!l->order)
		return;
	for (size_t i = 0; i < t->a->nfindings; i++)
		l->order[l->count++] = i;
	sorting = t->a;
	if (l->count > 1)
		qsort(l->order, l->count, sizeof *l->order, by_size);
}

static const struct finding *finding_at(const struct tui *t, size_t i)
{
	return &t->a->findings[t->free.order[i]];
}

static void draw_finding(struct tui *t, size_t i, int y)
{
	const struct finding *fd = finding_at(t, i);
	bool safe = fd->rule->safety == SAFETY_SAFE, selected = i == t->free.cursor;
	struct term_style style = selected ? tui_select : tui_plain;
	struct term_style tag = selected ? tui_select : safe ? tui_safe : tui_check;
	char size[FMT_LEN], line[256];
	fmt_size(fd->size, size);
	term_fill(0, y, term_width(), style);
	term_put(10 - term_text_width(size), y, 9, style, size);
	term_put(12, y, 6, tag, safe ? "safe" : "check");
	if (fd->count > 1)
		snprintf(line, sizeof line, "%s  (%zu folders)", fd->rule->what, fd->count);
	else
		snprintf(line, sizeof line, "%s", fd->rule->what);
	term_put(19, y, term_width() - 19, style, line);
}

static void draw_detail(const struct finding *fd, int y)
{
	const char *me = getenv("HOME");
	const char *user = !strncmp(fd->home, "/home/", 6) && (!me || strcmp(me, fd->home)) ? fd->home + 6 : NULL;
	char line[PATH_MAX + 64], path[PATH_MAX], size[FMT_LEN];
	term_fill(0, y, term_width(), tui_dim);
	term_put(1, y, term_width() - 1, tui_dim, fd->rule->safety == SAFETY_SAFE ? "safe: rebuilt or downloaded again when needed" : "check: read what is inside before deleting");
	snprintf(line, sizeof line, "run: %s%s%s%s", user ? "(as " : "", user ? user : "", user ? ") " : "", fd->rule->how);
	term_put(1, y + 1, term_width() - 1, tui_bold, line);
	for (size_t i = 0; i < FINDING_PATHS && fd->paths[i]; i++) {
		node_path(fd->paths[i], path, sizeof path);
		fmt_size(node_size(fd->paths[i]), size);
		snprintf(line, sizeof line, "%s %9s  %s", i ? "    " : "at: ", size, path);
		term_put(1, y + 2 + (int)i, term_width() - 1, tui_plain, line);
	}
	if (fd->count > FINDING_PATHS) {
		snprintf(line, sizeof line, "     ... and %zu more", fd->count - FINDING_PATHS);
		term_put(1, y + 2 + FINDING_PATHS, term_width() - 1, tui_dim, line);
	}
}

static void draw_totals(struct tui *t, int y)
{
	char safe[FMT_LEN], check[FMT_LEN], line[256];
	fmt_size(t->a->safe, safe);
	fmt_size(t->a->check, check);
	snprintf(line, sizeof line, "%s safe, %s to check.  enter = show it in Files   (open files and duplicates: see Report)", safe, check);
	term_fill(0, y, term_width(), tui_dim);
	term_put(1, y, term_width() - 1, tui_dim, line);
}

void free_draw(struct tui *t, int y, int h)
{
	struct free_list *l = &t->free;
	if (!l->count) {
		term_put(2, y + 1, term_width() - 2, tui_plain, "Nothing known to be safe to delete was found here.");
		return;
	}
	draw_totals(t, y);
	size_t rows = h > DETAIL_LINES + 4 ? (size_t)(h - DETAIL_LINES - 2) : 1;
	if (l->cursor < l->top)
		l->top = l->cursor;
	if (l->cursor >= l->top + rows)
		l->top = l->cursor - rows + 1;
	size_t shown = 0;
	for (; shown < rows && l->top + shown < l->count; shown++)
		draw_finding(t, l->top + shown, y + 1 + (int)shown);
	if (h > DETAIL_LINES + 4)
		draw_detail(finding_at(t, l->cursor), y + 2 + (int)shown);
}

bool free_key(struct tui *t, int key)
{
	struct free_list *l = &t->free;
	size_t page = tui_body_height() > DETAIL_LINES + 4 ? (size_t)(tui_body_height() - DETAIL_LINES - 2) : 1;
	if (tui_move(&l->cursor, &l->top, l->count, page, key))
		return true;
	if ((key != TERM_KEY_ENTER && key != TERM_KEY_RIGHT) || !l->count)
		return false;
	const struct node *n = finding_at(t, l->cursor)->paths[0];
	if (n)
		files_reveal(t, n);
	return true;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tui_internal.h"

#define LINE_BYTES 4096

void pager_free(struct pager *p)
{
	free(p->data);
	free(p->lines);
	*p = (struct pager){ 0 };
}

void pager_load(struct pager *p, char *data, size_t size)
{
	free(p->data);
	free(p->lines);
	*p = (struct pager){ .data = data, .size = size };
	size_t cap = 0;
	for (size_t start = 0; start < size;) {
		if (p->nlines == cap) {
			cap = cap ? cap * 2 : 256;
			size_t *grown = realloc(p->lines, cap * sizeof *grown);
			if (!grown)
				return;
			p->lines = grown;
		}
		p->lines[p->nlines++] = start;
		char *nl = memchr(data + start, '\n', size - start);
		start = nl ? (size_t)(nl - data) + 1 : size;
	}
}

static size_t copy_line(const struct pager *p, size_t i, size_t skip, char *out)
{
	size_t start = p->lines[i], end = i + 1 < p->nlines ? p->lines[i + 1] : p->size;
	while (end > start && (p->data[end - 1] == '\n' || p->data[end - 1] == '\r'))
		end--;
	for (size_t col = 0; start < end && col < skip; col++)
		for (start++; start < end && (p->data[start] & 0xc0) == 0x80;)
			start++;
	size_t len = end - start < LINE_BYTES - 1 ? end - start : LINE_BYTES - 1;
	memcpy(out, p->data + start, len);
	out[len] = '\0';
	return len;
}

static bool is_heading(const char *line, const char *next)
{
	return next && !strncmp(next, "=====", 5) && line[0] && line[0] != ' ';
}

void pager_draw(struct pager *p, int y, int h)
{
	char line[LINE_BYTES], next[LINE_BYTES];
	for (int row = 0; row < h; row++) {
		size_t i = p->top + (size_t)row;
		term_fill(0, y + row, term_width(), tui_plain);
		if (i >= p->nlines)
			continue;
		copy_line(p, i, p->left, line);
		bool more = i + 1 < p->nlines && (copy_line(p, i + 1, 0, next), true);
		struct term_style style = p->has_match && p->match == i ? tui_select
					  : is_heading(line, more ? next : NULL) ? tui_bold
					  : !strncmp(line, "=====", 5)		 ? tui_dim
										 : tui_plain;
		term_put(1, y + row, term_width() - 1, style, line);
	}
}

static bool line_has(const struct pager *p, size_t i, const char *what)
{
	char line[LINE_BYTES];
	copy_line(p, i, 0, line);
	return strcasestr(line, what) != NULL;
}

void pager_search(struct tui *t, struct pager *p)
{
	if (!p->search[0] || !p->nlines)
		return;
	size_t from = p->has_match ? p->match + 1 : p->top;
	for (size_t k = 0; k < p->nlines; k++) {
		size_t i = (from + k) % p->nlines;
		if (!line_has(p, i, p->search))
			continue;
		p->match = i;
		p->has_match = true;
		p->top = i > 2 ? i - 2 : 0;
		tui_status(t, "found \"%s\" on line %zu", p->search, i + 1);
		return;
	}
	p->has_match = false;
	tui_status(t, "\"%s\" not found", p->search);
}

static void on_search(struct tui *t, bool done)
{
	struct pager *p = t->view == VIEW_HELP ? &t->help : &t->report;
	if (!done)
		return;
	snprintf(p->search, sizeof p->search, "%s", t->input);
	p->has_match = false;
	pager_search(t, p);
}

bool pager_key(struct tui *t, struct pager *p, int key)
{
	size_t page = (size_t)tui_body_height();
	size_t last = p->nlines > page ? p->nlines - page : 0;
	switch (key) {
	case TERM_KEY_UP: case 'k': p->top -= p->top > 0; return true;
	case TERM_KEY_DOWN: case 'j': p->top += p->top < last; return true;
	case TERM_KEY_PAGE_UP: p->top = p->top > page ? p->top - page : 0; return true;
	case TERM_KEY_PAGE_DOWN: case ' ': p->top = p->top + page < last ? p->top + page : last; return true;
	case TERM_KEY_HOME: case 'g': p->top = 0; return true;
	case TERM_KEY_END: case 'G': p->top = last; return true;
	case TERM_KEY_LEFT: p->left = p->left > 8 ? p->left - 8 : 0; return true;
	case TERM_KEY_RIGHT: p->left += 8; return true;
	case '/': tui_ask(t, "find: ", p->search, on_search); return true;
	case 'n': pager_search(t, p); return true;
	default: return false;
	}
}

#ifndef DISKMOON_TERM_H
#define DISKMOON_TERM_H

#include <stdbool.h>
#include <stdint.h>

#include "c-tuikit.h"

enum term_key {
	TERM_KEY_NONE = PNL_KEY_NONE,
	TERM_KEY_UP = PNL_KEY_UP,
	TERM_KEY_DOWN = PNL_KEY_DOWN,
	TERM_KEY_LEFT = PNL_KEY_LEFT,
	TERM_KEY_RIGHT = PNL_KEY_RIGHT,
	TERM_KEY_HOME = PNL_KEY_HOME,
	TERM_KEY_END = PNL_KEY_END,
	TERM_KEY_PAGE_UP = PNL_KEY_PAGE_UP,
	TERM_KEY_PAGE_DOWN = PNL_KEY_PAGE_DOWN,
	TERM_KEY_DELETE = PNL_KEY_DELETE,
	TERM_KEY_ENTER = PNL_KEY_ENTER,
	TERM_KEY_BACKSPACE = PNL_KEY_BACKSPACE,
	TERM_KEY_ESCAPE = PNL_KEY_ESCAPE,
	TERM_KEY_TAB = PNL_KEY_TAB,
	TERM_KEY_BACKTAB = PNL_KEY_BACKTAB,
};

#define TERM_CTRL(c) ((c) & 0x1f)

enum term_attr {
	TERM_BOLD = PNL_BOLD,
	TERM_DIM = PNL_DIM,
	TERM_REVERSE = PNL_REVERSE,
	TERM_UNDERLINE = PNL_UNDERLINE,
};

enum term_color { TERM_DEFAULT, TERM_BLACK, TERM_RED, TERM_GREEN, TERM_YELLOW, TERM_BLUE, TERM_MAGENTA, TERM_CYAN, TERM_WHITE };

struct term_style {
	uint8_t attr;
	uint8_t fg;
	uint8_t bg;
};

void term_begin(pnl_canvas_t *canvas);
int term_width(void);
int term_height(void);
int term_text_width(const char *text);
int term_put(int x, int y, int max, struct term_style style, const char *text);
void term_fill(int x, int y, int width, struct term_style style);
void term_cursor(int x, int y);

#endif

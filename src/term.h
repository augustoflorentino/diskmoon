#ifndef DISKMOON_TERM_H
#define DISKMOON_TERM_H

#include <stdbool.h>
#include <stdint.h>

enum term_key {
	TERM_KEY_NONE = 0,
	TERM_KEY_UP = 0x110000,
	TERM_KEY_DOWN,
	TERM_KEY_LEFT,
	TERM_KEY_RIGHT,
	TERM_KEY_HOME,
	TERM_KEY_END,
	TERM_KEY_PAGE_UP,
	TERM_KEY_PAGE_DOWN,
	TERM_KEY_DELETE,
	TERM_KEY_ENTER,
	TERM_KEY_BACKSPACE,
	TERM_KEY_ESCAPE,
	TERM_KEY_TAB,
	TERM_KEY_BACKTAB,
	TERM_KEY_RESIZE,
};

#define TERM_CTRL(c) ((c) & 0x1f)

enum term_attr {
	TERM_BOLD = 1 << 0,
	TERM_DIM = 1 << 1,
	TERM_REVERSE = 1 << 2,
	TERM_UNDERLINE = 1 << 3,
};

enum term_color { TERM_DEFAULT, TERM_BLACK, TERM_RED, TERM_GREEN, TERM_YELLOW, TERM_BLUE, TERM_MAGENTA, TERM_CYAN, TERM_WHITE };

struct term_style {
	uint8_t attr;
	uint8_t fg;
	uint8_t bg;
};

bool term_open(void);
void term_close(void);
int term_width(void);
int term_height(void);
void term_clear(void);
int term_text_width(const char *text);
int term_put(int x, int y, int max, struct term_style style, const char *text);
void term_fill(int x, int y, int width, struct term_style style);
void term_cursor(int x, int y, bool visible);
void term_flush(void);
void term_redraw(void);
int term_read_key(int timeout_ms);

#endif

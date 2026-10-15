#include <string.h>

#include "term.h"

static pnl_canvas_t *canvas;
static int width, height;

void term_begin(pnl_canvas_t *c)
{
	canvas = c;
	width = pnl_canvas_width(c);
	height = pnl_canvas_height(c);
}

int term_width(void)
{
	return width;
}

int term_height(void)
{
	return height;
}

static int style_of(struct term_style s)
{
	return s.attr | PNL_FG(s.fg) | PNL_BG(s.bg);
}

static size_t sequence(const unsigned char *s)
{
	size_t len = *s < 0x80 ? 1 : *s >= 0xc2 && *s < 0xe0 ? 2 : *s >= 0xe0 && *s < 0xf0 ? 3 : *s >= 0xf0 && *s < 0xf5 ? 4 : 0;
	for (size_t i = 1; i < len; i++)
		if ((s[i] & 0xc0) != 0x80)
			return 0;
	return len;
}

static int glyph(const unsigned char *s, size_t *len, char out[5])
{
	*len = sequence(s);
	if (!*len) {
		*len = 1;
		return -1;
	}
	memcpy(out, s, *len);
	out[*len] = '\0';
	return *s < 0x20 || *s == 0x7f ? -1 : pnl_text_width(out);
}

int term_text_width(const char *text)
{
	int total = 0;
	char one[5];
	for (const unsigned char *s = (const unsigned char *)text; *s;) {
		size_t len;
		int w = glyph(s, &len, one);
		total += w < 0 ? 1 : w;
		s += len;
	}
	return total;
}

int term_put(int x, int y, int max, struct term_style style, const char *text)
{
	int limit = x + max < term_width() ? x + max : term_width();
	int col = x;
	char one[5];
	if (y < 0 || y >= term_height() || x < 0)
		return 0;
	for (const unsigned char *s = (const unsigned char *)text; *s && col < limit;) {
		size_t len;
		int w = glyph(s, &len, one);
		if (w < 0) {
			pnl_canvas_text(canvas, col++, y, style_of(style) | PNL_REVERSE, "?");
		} else if (w > 0 && col + w <= limit) {
			pnl_canvas_text(canvas, col, y, style_of(style), one);
			col += w;
		} else if (w > 0) {
			break;
		}
		s += len;
	}
	return col - x;
}

void term_fill(int x, int y, int width, struct term_style style)
{
	pnl_canvas_fill(canvas, x, y, width, 1, style_of(style));
}

void term_cursor(int x, int y)
{
	pnl_canvas_fill(canvas, x, y, 1, 1, PNL_REVERSE);
}

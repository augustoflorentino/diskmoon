#include <errno.h>
#include <locale.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#include "term.h"

#define OUT_CAP (1u << 16)
#define ESCAPE_WAIT_MS 25

struct cell {
	char bytes[4];
	uint8_t len;
	struct term_style style;
};

static struct {
	bool open;
	struct termios saved;
	int width, height;
	struct cell *back, *front;
	bool full_redraw;
	int cursor_x, cursor_y;
	bool cursor_visible;
	char out[OUT_CAP];
	size_t outlen;
	unsigned char in[64];
	size_t inlen;
} term;

static volatile sig_atomic_t resized;

static void out_flush(void)
{
	size_t done = 0;
	while (done < term.outlen) {
		ssize_t n = write(STDOUT_FILENO, term.out + done, term.outlen - done);
		if (n < 0 && errno == EINTR)
			continue;
		if (n <= 0)
			break;
		done += (size_t)n;
	}
	term.outlen = 0;
}

static void out(const char *s, size_t len)
{
	if (term.outlen + len > OUT_CAP)
		out_flush();
	memcpy(term.out + term.outlen, s, len);
	term.outlen += len;
}

static void outs(const char *s)
{
	out(s, strlen(s));
}

static void on_resize(int sig)
{
	(void)sig;
	resized = 1;
}

static void on_fatal(int sig)
{
	term_close();
	signal(sig, SIG_DFL);
	raise(sig);
}

static bool alloc_screen(void)
{
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) || !ws.ws_col || !ws.ws_row)
		ws = (struct winsize){ .ws_col = 80, .ws_row = 24 };
	size_t cells = (size_t)ws.ws_col * ws.ws_row;
	struct cell *back = realloc(term.back, cells * sizeof *back);
	struct cell *front = back ? realloc(term.front, cells * sizeof *front) : NULL;
	if (back)
		term.back = back;
	if (!front)
		return false;
	term.front = front;
	term.width = ws.ws_col;
	term.height = ws.ws_row;
	term.full_redraw = true;
	term_clear();
	return true;
}

bool term_open(void)
{
	if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO) || tcgetattr(STDIN_FILENO, &term.saved))
		return false;
	setlocale(LC_CTYPE, "");
	if (!alloc_screen())
		return false;
	struct termios raw = term.saved;
	raw.c_iflag &= ~(unsigned)(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(unsigned)OPOST;
	raw.c_cflag |= CS8;
	raw.c_lflag &= ~(unsigned)(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	struct sigaction sa = { .sa_handler = on_resize };
	sigaction(SIGWINCH, &sa, NULL);
	signal(SIGTERM, on_fatal);
	signal(SIGHUP, on_fatal);
	atexit(term_close);
	term.open = true;
	outs("\033[?1049h\033[?25l\033[H\033[2J");
	out_flush();
	return true;
}

void term_close(void)
{
	if (!term.open)
		return;
	term.open = false;
	outs("\033[0m\033[?25h\033[?1049l");
	out_flush();
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term.saved);
}

int term_width(void)
{
	return term.width;
}

int term_height(void)
{
	return term.height;
}

void term_clear(void)
{
	size_t cells = (size_t)term.width * term.height;
	for (size_t i = 0; i < cells; i++)
		term.back[i] = (struct cell){ .bytes = " ", .len = 1 };
}

static size_t decode(const unsigned char *s, wchar_t *wc)
{
	unsigned c = s[0];
	size_t len = c < 0x80 ? 1 : (c & 0xe0) == 0xc0 ? 2 : (c & 0xf0) == 0xe0 ? 3 : (c & 0xf8) == 0xf0 ? 4 : 0;
	if (!len)
		return 0;
	unsigned value = len == 1 ? c : c & (0x7f >> len);
	for (size_t i = 1; i < len; i++) {
		if ((s[i] & 0xc0) != 0x80)
			return 0;
		value = value << 6 | (s[i] & 0x3f);
	}
	static const unsigned least[] = { 0, 0, 0x80, 0x800, 0x10000 };
	if (value < least[len] || value > 0x10ffff || (value >= 0xd800 && value < 0xe000))
		return 0;
	*wc = (wchar_t)value;
	return len;
}

static int glyph(const unsigned char *s, size_t *len)
{
	wchar_t wc;
	*len = decode(s, &wc);
	if (!*len) {
		*len = 1;
		return -1;
	}
	int w = wcwidth(wc);
	return w < 0 ? -1 : w;
}

int term_text_width(const char *text)
{
	int total = 0;
	for (const unsigned char *s = (const unsigned char *)text; *s;) {
		size_t len;
		int w = glyph(s, &len);
		total += w < 0 ? 1 : w;
		s += len;
	}
	return total;
}

static struct cell *cell_at(int x, int y)
{
	return &term.back[(size_t)y * term.width + x];
}

static void set_cell(int x, int y, const unsigned char *bytes, size_t len, struct term_style style)
{
	struct cell *c = cell_at(x, y);
	memcpy(c->bytes, bytes, len);
	c->len = (uint8_t)len;
	c->style = style;
}

int term_put(int x, int y, int max, struct term_style style, const char *text)
{
	if (y < 0 || y >= term.height || x < 0)
		return 0;
	int limit = x + max < term.width ? x + max : term.width;
	int col = x;
	for (const unsigned char *s = (const unsigned char *)text; *s && col < limit;) {
		size_t len;
		int w = glyph(s, &len);
		if (w < 0) {
			struct term_style odd = { style.attr | TERM_REVERSE, style.fg, style.bg };
			set_cell(col++, y, (const unsigned char *)"?", 1, odd);
		} else if (w > 0 && col + w <= limit) {
			set_cell(col, y, s, len, style);
			for (int i = 1; i < w; i++)
				set_cell(col + i, y, (const unsigned char *)"", 0, style);
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
	if (y < 0 || y >= term.height)
		return;
	for (int i = x < 0 ? 0 : x; i < x + width && i < term.width; i++)
		set_cell(i, y, (const unsigned char *)" ", 1, style);
}

void term_cursor(int x, int y, bool visible)
{
	term.cursor_x = x;
	term.cursor_y = y;
	term.cursor_visible = visible;
}

static bool same_cell(const struct cell *a, const struct cell *b)
{
	return a->len == b->len && !memcmp(a->bytes, b->bytes, a->len) && a->style.attr == b->style.attr &&
	       a->style.fg == b->style.fg && a->style.bg == b->style.bg;
}

static void emit_style(struct term_style s)
{
	char buf[48];
	int n = snprintf(buf, sizeof buf, "\033[0%s%s%s%s", s.attr & TERM_BOLD ? ";1" : "", s.attr & TERM_DIM ? ";2" : "",
			 s.attr & TERM_UNDERLINE ? ";4" : "", s.attr & TERM_REVERSE ? ";7" : "");
	if (s.fg)
		n += snprintf(buf + n, sizeof buf - (size_t)n, ";%d", 29 + s.fg);
	if (s.bg)
		n += snprintf(buf + n, sizeof buf - (size_t)n, ";%d", 39 + s.bg);
	out(buf, (size_t)n);
	out("m", 1);
}

static void emit_move(int x, int y)
{
	char buf[32];
	out(buf, (size_t)snprintf(buf, sizeof buf, "\033[%d;%dH", y + 1, x + 1));
}

static void flush_row(int y, struct term_style *style, bool *styled)
{
	int next = -1;
	for (int x = 0; x < term.width; x++) {
		struct cell *b = cell_at(x, y), *f = &term.front[(size_t)y * term.width + x];
		if (!term.full_redraw && same_cell(b, f))
			continue;
		*f = *b;
		if (!b->len)
			continue;
		if (x != next)
			emit_move(x, y);
		if (!*styled || memcmp(style, &b->style, sizeof *style)) {
			emit_style(b->style);
			*style = b->style;
			*styled = true;
		}
		out(b->bytes, b->len);
		int w = 1;
		while (x + w < term.width && !cell_at(x + w, y)->len)
			w++;
		next = x + w;
	}
}

void term_flush(void)
{
	struct term_style style = { 0 };
	bool styled = false;
	outs("\033[?25l");
	if (term.full_redraw)
		outs("\033[0m\033[2J");
	for (int y = 0; y < term.height; y++)
		flush_row(y, &style, &styled);
	term.full_redraw = false;
	outs("\033[0m");
	if (term.cursor_visible) {
		emit_move(term.cursor_x, term.cursor_y);
		outs("\033[?25h");
	}
	out_flush();
}

void term_redraw(void)
{
	term.full_redraw = true;
}

static bool fill_input(int timeout_ms)
{
	struct pollfd p = { .fd = STDIN_FILENO, .events = POLLIN };
	if (poll(&p, 1, timeout_ms) <= 0)
		return false;
	ssize_t n = read(STDIN_FILENO, term.in + term.inlen, sizeof term.in - term.inlen);
	if (n <= 0)
		return false;
	term.inlen += (size_t)n;
	return true;
}

static void consume(size_t n)
{
	memmove(term.in, term.in + n, term.inlen - n);
	term.inlen -= n;
}

static int tilde_key(int code)
{
	switch (code) {
	case 1: case 7: return TERM_KEY_HOME;
	case 4: case 8: return TERM_KEY_END;
	case 3: return TERM_KEY_DELETE;
	case 5: return TERM_KEY_PAGE_UP;
	case 6: return TERM_KEY_PAGE_DOWN;
	default: return TERM_KEY_NONE;
	}
}

static int final_key(unsigned char c)
{
	switch (c) {
	case 'A': return TERM_KEY_UP;
	case 'B': return TERM_KEY_DOWN;
	case 'C': return TERM_KEY_RIGHT;
	case 'D': return TERM_KEY_LEFT;
	case 'H': return TERM_KEY_HOME;
	case 'F': return TERM_KEY_END;
	case 'Z': return TERM_KEY_BACKTAB;
	default: return TERM_KEY_NONE;
	}
}

static int parse_escape(void)
{
	while (term.inlen < 3 && fill_input(ESCAPE_WAIT_MS))
		;
	if (term.inlen < 2 || (term.in[1] != '[' && term.in[1] != 'O')) {
		consume(1);
		return TERM_KEY_ESCAPE;
	}
	size_t i = 2;
	int code = 0;
	for (;; i++) {
		if (i >= term.inlen && !fill_input(ESCAPE_WAIT_MS)) {
			consume(term.inlen);
			return TERM_KEY_NONE;
		}
		unsigned char c = term.in[i];
		if (c >= '0' && c <= '9')
			code = code * 10 + (c - '0');
		else if (c != ';')
			break;
	}
	unsigned char last = term.in[i];
	consume(i + 1);
	return last == '~' ? tilde_key(code) : final_key(last);
}

static int parse_key(void)
{
	unsigned char c = term.in[0];
	if (c == 27)
		return parse_escape();
	consume(1);
	switch (c) {
	case '\r': case '\n': return TERM_KEY_ENTER;
	case 127: case 8: return TERM_KEY_BACKSPACE;
	case '\t': return TERM_KEY_TAB;
	}
	if (c < 0x80)
		return c;
	size_t need = (c & 0xe0) == 0xc0 ? 1 : (c & 0xf0) == 0xe0 ? 2 : (c & 0xf8) == 0xf0 ? 3 : 0;
	while (term.inlen < need && fill_input(ESCAPE_WAIT_MS))
		;
	unsigned char seq[5] = { c };
	memcpy(seq + 1, term.in, need < term.inlen ? need : term.inlen);
	wchar_t wc;
	size_t got = decode(seq, &wc);
	consume(need < term.inlen ? need : term.inlen);
	return got ? (int)wc : TERM_KEY_NONE;
}

int term_read_key(int timeout_ms)
{
	if (resized) {
		resized = 0;
		alloc_screen();
		return TERM_KEY_RESIZE;
	}
	if (!term.inlen && !fill_input(timeout_ms))
		return resized ? term_read_key(0) : TERM_KEY_NONE;
	return parse_key();
}

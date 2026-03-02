#include <errno.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "import.h"
#include "inodes.h"
#include "mounts.h"

#define READ_BUFFER (1u << 20)
#define TEXT_MAX (PATH_MAX * 4)

struct entry {
	uint64_t asize;
	uint64_t dsize;
	uint64_t dev;
	uint64_t ino;
	uint64_t mtime;
	bool has_dev;
	bool hardlink;
	bool notreg;
	bool read_error;
	bool excluded;
	char name[TEXT_MAX];
};

struct reader {
	FILE *f;
	const struct options *o;
	struct scan_result *r;
	struct arena arena;
	jmp_buf fail;
	const char *problem;
	uint64_t offset;
	size_t pos;
	size_t len;
	struct entry entry;
	char text[TEXT_MAX];
	char buf[READ_BUFFER];
};

static _Noreturn void bad(struct reader *in, const char *problem)
{
	in->problem = problem;
	longjmp(in->fail, 1);
}

static int peek(struct reader *in)
{
	if (in->pos == in->len) {
		in->offset += in->len;
		in->len = fread_unlocked(in->buf, 1, sizeof in->buf, in->f);
		in->pos = 0;
		if (!in->len)
			return EOF;
	}
	return (unsigned char)in->buf[in->pos];
}

static int next(struct reader *in)
{
	int c = peek(in);
	if (c != EOF)
		in->pos++;
	return c;
}

static int skip_space(struct reader *in)
{
	int c;
	while ((c = peek(in)) == ' ' || c == '\n' || c == '\r' || c == '\t')
		in->pos++;
	return c;
}

static void expect(struct reader *in, int want)
{
	if (skip_space(in) != want)
		bad(in, "unexpected character");
	in->pos++;
}

static bool accept(struct reader *in, int want)
{
	if (skip_space(in) != want)
		return false;
	in->pos++;
	return true;
}

static unsigned hex4(struct reader *in)
{
	unsigned v = 0;
	for (int i = 0; i < 4; i++) {
		int c = next(in);
		int d = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
		if (d < 0)
			bad(in, "bad \\u escape");
		v = v << 4 | (unsigned)d;
	}
	return v;
}

static size_t put_utf8(char *out, unsigned cp)
{
	if (cp < 0x80)
		return out[0] = (char)cp, 1;
	if (cp < 0x800)
		return out[0] = (char)(0xc0 | cp >> 6), out[1] = (char)(0x80 | (cp & 0x3f)), 2;
	if (cp < 0x10000)
		return out[0] = (char)(0xe0 | cp >> 12), out[1] = (char)(0x80 | (cp >> 6 & 0x3f)),
		       out[2] = (char)(0x80 | (cp & 0x3f)), 3;
	return out[0] = (char)(0xf0 | cp >> 18), out[1] = (char)(0x80 | (cp >> 12 & 0x3f)),
	       out[2] = (char)(0x80 | (cp >> 6 & 0x3f)), out[3] = (char)(0x80 | (cp & 0x3f)), 4;
}

static unsigned read_codepoint(struct reader *in)
{
	unsigned cp = hex4(in);
	if (cp < 0xd800 || cp > 0xdbff)
		return cp;
	if (next(in) != '\\' || next(in) != 'u')
		bad(in, "broken surrogate pair");
	unsigned low = hex4(in);
	return 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
}

static char escape_char(int c)
{
	switch (c) {
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	default: return (char)c;
	}
}

static void read_string(struct reader *in, char *out)
{
	size_t len = 0;
	expect(in, '"');
	for (;;) {
		int c = next(in);
		if (c == EOF)
			bad(in, "unterminated string");
		if (c == '"')
			break;
		if (len + 4 >= TEXT_MAX)
			bad(in, "string too long");
		if (c != '\\') {
			out[len++] = (char)c;
			continue;
		}
		c = next(in);
		len += c == 'u' ? put_utf8(out + len, read_codepoint(in)) : (out[len] = escape_char(c), 1);
	}
	out[len] = '\0';
}

static uint64_t read_number(struct reader *in)
{
	uint64_t v = 0;
	int c = skip_space(in);
	bool negative = c == '-';
	if (negative)
		in->pos++;
	if (peek(in) < '0' || peek(in) > '9')
		bad(in, "expected a number");
	while ((c = peek(in)) >= '0' && c <= '9') {
		v = v * 10 + (uint64_t)(c - '0');
		in->pos++;
	}
	while ((c = peek(in)) == '.' || c == 'e' || c == 'E' || c == '+' || c == '-' || (c >= '0' && c <= '9'))
		in->pos++;
	return negative ? 0 : v;
}

static void read_word(struct reader *in, const char *word)
{
	skip_space(in);
	for (const char *w = word; *w; w++)
		if (next(in) != *w)
			bad(in, "unexpected literal");
}

static bool read_bool(struct reader *in)
{
	if (skip_space(in) == 't')
		return read_word(in, "true"), true;
	if (peek(in) == 'f')
		return read_word(in, "false"), false;
	read_word(in, "null");
	return false;
}

static void skip_value(struct reader *in, int depth);

static void skip_container(struct reader *in, int close, int depth)
{
	in->pos++;
	if (accept(in, close))
		return;
	do {
		if (close == '}') {
			read_string(in, in->text);
			expect(in, ':');
		}
		skip_value(in, depth + 1);
	} while (accept(in, ','));
	expect(in, close);
}

static void skip_value(struct reader *in, int depth)
{
	int c = skip_space(in);
	if (depth > 256)
		bad(in, "nesting too deep");
	if (c == '"')
		read_string(in, in->text);
	else if (c == '{' || c == '[')
		skip_container(in, c == '{' ? '}' : ']', depth);
	else if (c == 't' || c == 'f' || c == 'n')
		read_bool(in);
	else
		read_number(in);
}

static void read_field(struct reader *in, struct entry *e, const char *key)
{
	if (!strcmp(key, "name"))
		read_string(in, e->name);
	else if (!strcmp(key, "asize"))
		e->asize = read_number(in);
	else if (!strcmp(key, "dsize"))
		e->dsize = read_number(in);
	else if (!strcmp(key, "dev"))
		e->dev = read_number(in), e->has_dev = true;
	else if (!strcmp(key, "ino"))
		e->ino = read_number(in);
	else if (!strcmp(key, "mtime"))
		e->mtime = read_number(in);
	else if (!strcmp(key, "hlnkc"))
		e->hardlink = read_bool(in);
	else if (!strcmp(key, "notreg"))
		e->notreg = read_bool(in);
	else if (!strcmp(key, "read_error"))
		e->read_error = read_bool(in);
	else if (!strcmp(key, "excluded"))
		e->excluded = true, skip_value(in, 0);
	else
		skip_value(in, 0);
}

static void read_entry(struct reader *in, struct entry *e)
{
	memset(e, 0, offsetof(struct entry, name));
	e->name[0] = '\0';
	expect(in, '{');
	if (accept(in, '}'))
		bad(in, "entry without a name");
	do {
		read_string(in, in->text);
		expect(in, ':');
		read_field(in, e, in->text);
	} while (accept(in, ','));
	expect(in, '}');
	if (!e->name[0])
		bad(in, "entry without a name");
}

static uint64_t entry_size(const struct reader *in, const struct entry *e)
{
	return in->o->apparent ? e->asize : e->dsize;
}

static void record_error(struct reader *in, const struct node *parent, struct node *n)
{
	char path[LONG_PATH];
	struct scan_result *r = in->r;
	if (parent)
		node_path_in(parent, n, path, sizeof path);
	else
		node_path(n, path, sizeof path);
	r->errors = realloc(r->errors, (r->nerrors + 1) * sizeof *r->errors);
	if (!r->errors)
		bad(in, "out of memory");
	r->errors[r->nerrors++] = (struct scan_error){ strdup(path), EACCES };
}

static void attach_mount(struct node *n, uint64_t dev)
{
	char path[LONG_PATH];
	node_path(n, path, sizeof path);
	int index = mounts_add_imported(dev, path);
	if (index <= INT16_MAX)
		node_dir(n)->mount = (int16_t)index;
}

static struct node *make_node(struct reader *in, struct node *parent, const struct entry *e, bool dir, uint64_t dev)
{
	struct node *n = node_new(&in->arena, parent, e->name, dir, entry_size(in, e));
	n->mtime = e->mtime > UINT32_MAX ? UINT32_MAX : (uint32_t)e->mtime;
	if (e->notreg && !dir)
		n->flags |= NODE_LINK;
	if (e->excluded)
		n->flags |= NODE_SKIPPED;
	if (dir && parent && e->has_dev && e->dev != dev)
		n->flags |= NODE_MOUNT;
	if (dir && e->has_dev && (!parent || n->flags & NODE_MOUNT))
		attach_mount(n, e->dev);
	if (e->hardlink && (!e->ino || inodes_seen(e->has_dev ? e->dev : dev, e->ino))) {
		n->flags |= NODE_HARDLINK;
		node_set_size(n, 0);
	}
	if (parent)
		node_push_child(parent, n);
	if (e->read_error) {
		n->flags |= NODE_ERROR;
		record_error(in, parent, n);
	}
	*(dir ? &in->r->dirs : &in->r->files) += 1;
	return n;
}

static struct node *read_item(struct reader *in, struct node *parent, uint64_t dev, int depth);

static struct node *read_dir(struct reader *in, struct node *parent, uint64_t dev, int depth)
{
	expect(in, '[');
	read_entry(in, &in->entry);
	uint64_t own = in->entry.has_dev ? in->entry.dev : dev;
	struct node *n = make_node(in, parent, &in->entry, true, dev);
	while (accept(in, ','))
		read_item(in, n, own, depth + 1);
	expect(in, ']');
	return n;
}

static struct node *read_item(struct reader *in, struct node *parent, uint64_t dev, int depth)
{
	if (depth > PATH_MAX / 2)
		bad(in, "tree too deep");
	if (skip_space(in) == '[')
		return read_dir(in, parent, dev, depth);
	read_entry(in, &in->entry);
	return make_node(in, parent, &in->entry, false, dev);
}

static void read_metadata(struct reader *in)
{
	char prog[32] = "?", ver[32] = "?";
	expect(in, '{');
	if (accept(in, '}'))
		return;
	do {
		read_string(in, in->text);
		expect(in, ':');
		char *target = !strcmp(in->text, "progname") ? prog : !strcmp(in->text, "progver") ? ver : NULL;
		if (!strcmp(in->text, "timestamp"))
			in->r->started = (int64_t)read_number(in);
		else if (target) {
			read_string(in, in->text);
			snprintf(target, sizeof prog, "%.31s", in->text);
		} else
			skip_value(in, 0);
	} while (accept(in, ','));
	expect(in, '}');
	snprintf(in->r->producer, sizeof in->r->producer, "%s %s", prog, ver);
}

static void read_document(struct reader *in)
{
	expect(in, '[');
	if (read_number(in) != 1)
		bad(in, "unsupported major version (only 1 is known)");
	expect(in, ',');
	read_number(in);
	expect(in, ',');
	read_metadata(in);
	expect(in, ',');
	if (skip_space(in) != '[')
		bad(in, "the top entry is not a directory");
	in->r->root = read_dir(in, NULL, 0, 0);
	expect(in, ']');
}

static FILE *open_input(const char *name)
{
	if (!strcmp(name, "-"))
		return stdin;
	FILE *f = fopen(name, "r");
	if (!f)
		fprintf(stderr, "diskmoon: %s: %s\n", name, strerror(errno));
	return f;
}

int import_ncdu(const struct options *o, const char *file, struct scan_result *r)
{
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	memset(r, 0, sizeof *r);
	r->source = file;
	inodes_init();
	if (!node_space_init()) {
		fputs("diskmoon: cannot reserve memory for the tree\n", stderr);
		return -1;
	}
	struct reader *in = calloc(1, sizeof *in);
	if (!in || !(in->f = open_input(file))) {
		free(in);
		return -1;
	}
	in->o = o;
	in->r = r;
	int status = 0;
	if (setjmp(in->fail)) {
		fprintf(stderr, "diskmoon: %s: not a valid ncdu export: %s near byte %llu\n", file, in->problem,
			(unsigned long long)(in->offset + in->pos));
		status = -1;
	} else
		read_document(in);
	if (in->f != stdin)
		fclose(in->f);
	free(in);
	clock_gettime(CLOCK_MONOTONIC, &end);
	r->seconds = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
	r->jobs = 1;
	return status;
}

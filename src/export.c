#include <limits.h>
#include <string.h>

#include "config.h"
#include "export.h"
#include "fmt.h"
#include "mounts.h"

#define ENTRY_MAX (LONG_PATH * 6 + 256)
#define NAME_ENTRY_MAX (PATH_MAX * 6 + 256)

static char *put(char *out, const char *s)
{
	size_t len = strlen(s);
	memcpy(out, s, len);
	return out + len;
}

static char *put_u64(char *out, uint64_t v)
{
	char tmp[24];
	int len = 0;
	do
		tmp[len++] = (char)('0' + v % 10);
	while (v /= 10);
	while (len)
		*out++ = tmp[--len];
	return out;
}

static char *put_json_string(char *out, const char *s)
{
	static const char hex[] = "0123456789abcdef";
	*out++ = '"';
	for (const unsigned char *c = (const unsigned char *)s; *c; c++) {
		if (*c >= 0x20 && *c != '"' && *c != '\\') {
			*out++ = (char)*c;
			continue;
		}
		*out++ = '\\';
		if (*c == '"' || *c == '\\') {
			*out++ = (char)*c;
			continue;
		}
		out = put(out, "u00");
		*out++ = hex[*c >> 4];
		*out++ = hex[*c & 15];
	}
	*out++ = '"';
	return out;
}

static char *put_field(char *out, const char *key, uint64_t value)
{
	*out++ = ',';
	out = put(out, key);
	return put_u64(out, value);
}

static uint64_t own_size(const struct node *n)
{
	uint64_t size = node_size(n);
	if (n->flags & NODE_DIR)
		for (const struct node *c = node_child(n); c; c = node_next(c))
			size -= node_size(c);
	return size;
}

static char *put_ncdu_info(char *out, const struct options *o, const struct node *n)
{
	const struct mount *m = mounts_get(node_mount(n));
	out = put(out, "{\"name\":");
	out = put_json_string(out, n->name);
	out = put_field(out, o->apparent ? "\"asize\":" : "\"dsize\":", own_size(n));
	if (m && (node_is_root(n) || n->flags & NODE_MOUNT || m->subvolume))
		out = put_field(out, "\"dev\":", m->dev);
	if (n->mtime)
		out = put_field(out, "\"mtime\":", n->mtime);
	if (n->flags & NODE_LINK)
		out = put(out, ",\"notreg\":true");
	if (n->flags & NODE_ERROR)
		out = put(out, ",\"read_error\":true");
	if (n->flags & NODE_HARDLINK)
		out = put(out, ",\"hlnkc\":true");
	if (n->flags & NODE_SKIPPED)
		out = put(out, ",\"excluded\":\"otherfs\"");
	return put(out, "}");
}

static void ncdu_node(FILE *f, const struct options *o, const struct node *n)
{
	char line[NAME_ENTRY_MAX];
	char *p = line;
	*p++ = ',';
	*p++ = '\n';
	if (n->flags & NODE_DIR)
		*p++ = '[';
	p = put_ncdu_info(p, o, n);
	fwrite_unlocked(line, 1, (size_t)(p - line), f);
	if (!(n->flags & NODE_DIR))
		return;
	for (const struct node *c = node_child(n); c; c = node_next(c))
		ncdu_node(f, o, c);
	fputc_unlocked(']', f);
}

void export_ncdu(FILE *f, const struct options *o, const struct scan_result *r)
{
	char line[ENTRY_MAX];
	char *p = put(line, "[1,2,{\"progname\":\"diskmoon\",\"progver\":\"" DISKMOON_VERSION "\",\"timestamp\":");
	p = put_u64(p, (uint64_t)r->started);
	p = put(p, "},\n[");
	p = put_ncdu_info(p, o, r->root);
	fwrite_unlocked(line, 1, (size_t)(p - line), f);
	for (const struct node *c = node_child(r->root); c; c = node_next(c))
		ncdu_node(f, o, c);
	fputs("]]\n", f);
}

static char *put_utc(char *out, int64_t t)
{
	int64_t days = t / 86400, secs = t % 86400;
	if (secs < 0) {
		secs += 86400;
		days--;
	}
	int64_t z = days + 719468, era = (z >= 0 ? z : z - 146096) / 146097;
	int64_t doe = z - era * 146097, yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100), mp = (5 * doy + 2) / 153;
	int64_t d = doy - (153 * mp + 2) / 5 + 1, m = mp < 10 ? mp + 3 : mp - 9, y = yoe + era * 400 + (m <= 2);
	int64_t parts[6] = { y, m, d, secs / 3600, secs / 60 % 60, secs % 60 };
	static const char seps[] = "--T::Z";
	for (int i = 0; i < 6; i++) {
		if (parts[i] < 10)
			*out++ = '0';
		out = put_u64(out, (uint64_t)parts[i]);
		*out++ = seps[i];
	}
	return out;
}

static const char *csv_kind(const struct node *n)
{
	return n->flags & NODE_DIR ? "dir" : n->flags & NODE_LINK ? "link" : "file";
}

static const char *csv_note(const struct node *n)
{
	if (n->flags & NODE_MOUNT)
		return "mount";
	if (n->flags & NODE_SKIPPED)
		return "not scanned";
	if (n->flags & NODE_ERROR)
		return "unreadable";
	return n->flags & NODE_HARDLINK ? "hard link" : "";
}

static char *put_csv_path(char *out, const char *path)
{
	char escaped[LONG_PATH * 4 + 1];
	*fmt_escaped(escaped, path) = '\0';
	*out++ = '"';
	for (const char *c = escaped; *c; c++) {
		if (*c == '"')
			*out++ = '"';
		*out++ = *c;
	}
	*out++ = '"';
	return out;
}

static void csv_row(FILE *f, const struct node *n, const char *path)
{
	char line[ENTRY_MAX];
	char *p = put_csv_path(line, path);
	*p++ = ',';
	p = put(p, csv_kind(n));
	*p++ = ',';
	p = put_u64(p, node_size(n));
	*p++ = ',';
	p = put_u64(p, n->flags & NODE_DIR ? node_dir(n)->items : 0);
	*p++ = ',';
	if (n->mtime)
		p = put_utc(p, n->mtime);
	*p++ = ',';
	p = put(p, csv_note(n));
	*p++ = '\n';
	fwrite_unlocked(line, 1, (size_t)(p - line), f);
}

static bool csv_wanted(const struct options *o, const struct node *n)
{
	return node_size(n) >= o->min_size && (!o->no_files || n->flags & NODE_DIR);
}

static void csv_children(FILE *f, const struct options *o, const struct node *dir, char *path, size_t len, int depth)
{
	if (o->max_depth >= 0 && depth >= o->max_depth)
		return;
	for (const struct node *c = node_child(dir); c && node_size(c) >= o->min_size; c = node_next(c)) {
		size_t name = strlen(c->name);
		if (!csv_wanted(o, c) || len + name + 2 > LONG_PATH)
			continue;
		size_t at = len;
		if (path[len - 1] != '/')
			path[at++] = '/';
		memcpy(path + at, c->name, name + 1);
		csv_row(f, c, path);
		if (c->flags & NODE_DIR)
			csv_children(f, o, c, path, at + name, depth + 1);
		path[len] = '\0';
	}
}

void export_csv(FILE *f, const struct options *o, const struct scan_result *r)
{
	char path[LONG_PATH];
	snprintf(path, sizeof path, "%s", r->root->name);
	fputs("path,kind,size,items,modified,note\n", f);
	csv_row(f, r->root, path);
	csv_children(f, o, r->root, path, strlen(path), 0);
}

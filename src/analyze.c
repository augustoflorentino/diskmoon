#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "analyze.h"

#define DAY 86400
#define STALE_DAYS 365
#define MIN_FINDING (1u << 20)

static void *grow_or_die(void *p, size_t size)
{
	p = realloc(p, size);
	if (!p) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	return p;
}

static const struct node *child_named(const struct node *dir, const char *name, size_t len)
{
	for (const struct node *c = node_child(dir); c; c = node_next(c))
		if (!strncmp(c->name, name, len) && !c->name[len])
			return c;
	return NULL;
}

static const struct node *find_path(const struct node *root, const char *path)
{
	size_t rlen = strlen(root->name);
	if (strncmp(path, root->name, rlen) || (path[rlen] && path[rlen] != '/' && strcmp(root->name, "/")))
		return NULL;
	const struct node *n = root;
	for (const char *p = path + rlen; n && *p;) {
		while (*p == '/')
			p++;
		size_t len = strcspn(p, "/");
		if (len)
			n = child_named(n, p, len);
		p += len;
	}
	return n;
}

static struct finding *finding_for(struct analysis *a, const struct rule *rule, const char *home)
{
	for (size_t i = 0; i < a->nfindings; i++)
		if (a->findings[i].rule == rule && !strcmp(a->findings[i].home, home))
			return &a->findings[i];
	if (a->nfindings == a->capfindings) {
		a->capfindings = a->capfindings ? a->capfindings * 2 : 32;
		a->findings = grow_or_die(a->findings, a->capfindings * sizeof *a->findings);
	}
	struct finding *f = &a->findings[a->nfindings++];
	*f = (struct finding){ .rule = rule };
	snprintf(f->home, sizeof f->home, "%s", home);
	return f;
}

static void add_hit(struct analysis *a, const struct rule *rule, const char *home, struct node *n)
{
	struct finding *f = finding_for(a, rule, home);
	f->size += node_size(n);
	f->count++;
	n->flags |= NODE_CLAIMED;
	for (size_t i = 0; i < FINDING_PATHS; i++) {
		if (f->paths[i] && node_size(f->paths[i]) >= node_size(n))
			continue;
		memmove(&f->paths[i + 1], &f->paths[i], (FINDING_PATHS - i - 1) * sizeof *f->paths);
		f->paths[i] = n;
		break;
	}
}

static bool inside_claimed(const struct node *n)
{
	for (; n; n = node_parent(n))
		if (n->flags & NODE_CLAIMED)
			return true;
	return false;
}

static void apply_path_rule(struct analysis *a, const struct node *root, const struct rule *rule, const char *home)
{
	char path[PATH_MAX];
	if (rule->match[0] == '~' && !home)
		return;
	if (rule->match[0] == '~')
		snprintf(path, sizeof path, "%s%s", home, rule->match + 1);
	else
		snprintf(path, sizeof path, "%s", rule->match);
	struct node *n = (struct node *)find_path(root, path);
	if (n && node_size(n) && !inside_claimed(n))
		add_hit(a, rule, rule->match[0] == '~' ? home : "", n);
}

static size_t list_homes(const struct node *root, char homes[][PATH_MAX], size_t cap)
{
	size_t n = 0;
	const struct node *home = find_path(root, "/home");
	for (const struct node *c = home ? node_child(home) : NULL; c && n < cap; c = node_next(c))
		if (c->flags & NODE_DIR)
			snprintf(homes[n++], PATH_MAX, "/home/%s", c->name);
	const char *env = getenv("HOME");
	const char *extra[] = { "/root", env };
	for (size_t i = 0; i < 2 && n < cap; i++) {
		bool known = !extra[i] || !*extra[i];
		for (size_t j = 0; j < n && !known; j++)
			known = !strcmp(homes[j], extra[i]);
		if (!known && find_path(root, extra[i]))
			snprintf(homes[n++], PATH_MAX, "%s", extra[i]);
	}
	return n;
}

static void apply_path_rules(struct analysis *a, const struct node *root)
{
	static char homes[64][PATH_MAX];
	size_t nhomes = list_homes(root, homes, 64);
	for (const struct rule *rule = cleanup_rules; rule->what; rule++) {
		if (rule->kind != RULE_PATH)
			continue;
		if (rule->match[0] != '~')
			apply_path_rule(a, root, rule, NULL);
		for (size_t i = 0; i < nhomes && rule->match[0] == '~'; i++)
			apply_path_rule(a, root, rule, homes[i]);
	}
}

static bool stale_enough(const struct analysis *a, const struct node *n, unsigned days)
{
	return !days || (n->mtime && a->now - (int64_t)n->mtime >= (int64_t)days * DAY);
}

static bool inside_named(const struct node *n, const char *name)
{
	for (n = node_parent(n); n; n = node_parent(n))
		if (!strcmp(n->name, name))
			return true;
	return false;
}

static bool name_rule_matches(const struct analysis *a, const struct rule *rule, const struct node *n)
{
	if (rule->match && (strcmp(rule->match, n->name) || inside_named(n, rule->match)))
		return false;
	const struct node *where = rule->marker_in_parent ? node_parent(n) : n;
	if (rule->marker && (!where || !child_named(where, rule->marker, strlen(rule->marker))))
		return false;
	return stale_enough(a, n, rule->stale_days);
}

static const struct rule *name_rule_for(const struct analysis *a, const struct node *n)
{
	for (const struct rule *rule = cleanup_rules; rule->what; rule++)
		if (rule->kind == RULE_NAME && name_rule_matches(a, rule, n))
			return rule;
	return NULL;
}

static unsigned type_hash(const char *ext)
{
	unsigned h = 2166136261u;
	for (; *ext; ext++)
		h = (h ^ (unsigned char)*ext) * 16777619u;
	return h;
}

static void add_type(struct analysis *a, const struct node *n)
{
	char ext[16] = "";
	const char *dot = strrchr(n->name, '.');
	if (dot && dot != n->name && strlen(dot + 1) < sizeof ext && dot[1])
		for (size_t i = 0; dot[i + 1]; i++)
			ext[i] = (char)tolower((unsigned char)dot[i + 1]);
	for (unsigned i = type_hash(ext) % TYPE_SLOTS, tries = 0; tries < TYPE_SLOTS; i = (i + 1) % TYPE_SLOTS, tries++) {
		struct type_stat *t = &a->types[i];
		if (t->count && strcmp(t->ext, ext))
			continue;
		if (!t->count)
			memcpy(t->ext, ext, sizeof ext);
		t->size += node_size(n);
		t->count++;
		return;
	}
}

static void offer_stale(struct analysis *a, const struct node *n)
{
	size_t i = a->nstale;
	if (i == STALE_MAX && node_size(a->stale[STALE_MAX - 1]) >= node_size(n))
		return;
	if (i == STALE_MAX)
		i--;
	else
		a->nstale++;
	for (; i && node_size(a->stale[i - 1]) < node_size(n); i--)
		a->stale[i] = a->stale[i - 1];
	a->stale[i] = n;
}

static void visit(struct analysis *a, struct node *dir, bool claimed, bool in_stale)
{
	for (struct node *c = node_child(dir); c; c = node_next(c)) {
		if (!(c->flags & NODE_DIR)) {
			add_type(a, c);
			continue;
		}
		bool here = claimed || c->flags & NODE_CLAIMED;
		const struct rule *rule = here ? NULL : name_rule_for(a, c);
		if (rule && !rule->keep && node_size(c))
			add_hit(a, rule, "", c);
		bool stale = !in_stale && node_size(c) >= a->stale_floor && stale_enough(a, c, STALE_DAYS);
		if (stale)
			offer_stale(a, c);
		visit(a, c, here || rule, in_stale || stale);
	}
}

static void find_hot_path(struct analysis *a, const struct node *root)
{
	const struct node *n = root;
	a->nhot = 0;
	while (a->nhot < HOT_PATH_MAX) {
		const struct node *big = node_child(n);
		if (!big || !(big->flags & NODE_DIR) || node_size(big) * 2 < node_size(n))
			return;
		a->hot[a->nhot++] = big;
		n = big;
	}
}

static int by_finding_size(const void *x, const void *y)
{
	const struct finding *a = x, *b = y;
	return a->size == b->size ? 0 : a->size < b->size ? 1 : -1;
}

static int by_type_size(const void *x, const void *y)
{
	const struct type_stat *a = x, *b = y;
	return a->size == b->size ? 0 : a->size < b->size ? 1 : -1;
}

static void drop_small_findings(struct analysis *a)
{
	size_t out = 0;
	for (size_t i = 0; i < a->nfindings; i++) {
		if (a->findings[i].size < MIN_FINDING)
			continue;
		a->findings[out++] = a->findings[i];
		*(a->findings[i].rule->safety == SAFETY_SAFE ? &a->safe : &a->check) += a->findings[i].size;
	}
	a->nfindings = out;
	if (out > 1)
		qsort(a->findings, out, sizeof *a->findings, by_finding_size);
}

static void compact_types(struct analysis *a)
{
	size_t out = 0;
	for (size_t i = 0; i < TYPE_SLOTS; i++)
		if (a->types[i].count)
			a->types[out++] = a->types[i];
	a->ntypes = out;
	qsort(a->types, out, sizeof *a->types, by_type_size);
}

void analyze_run(const struct scan_result *r, struct analysis *a)
{
	memset(a, 0, sizeof *a);
	a->now = (int64_t)time(NULL);
	a->types = calloc(TYPE_SLOTS, sizeof *a->types);
	if (!a->types) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	a->stale_floor = node_size(r->root) / 100 > MIN_FINDING ? node_size(r->root) / 100 : MIN_FINDING;
	apply_path_rules(a, r->root);
	visit(a, r->root, r->root->flags & NODE_CLAIMED, false);
	drop_small_findings(a);
	compact_types(a);
	find_hot_path(a, r->root);
}

struct category {
	const char *name;
	const char *exts;
};

static const struct category categories[] = {
	{ "video", " mp4 mkv mov avi webm m4v flv wmv ts mts " },
	{ "image", " jpg jpeg png gif webp heic tif tiff bmp raw cr2 nef arw dng psd svg ico " },
	{ "audio", " mp3 flac wav ogg opus m4a aac wma " },
	{ "archive", " zip tar gz tgz xz zst bz2 7z rar lz4 lzma cab deb rpm apk " },
	{ "disk image", " img iso qcow2 vdi vmdk vhd vhdx raw dmg wim " },
	{ "AI model", " gguf safetensors ckpt pt pth onnx h5 tflite " },
	{ "database", " db sqlite sqlite3 sst ldb mdb ibd wal frm " },
	{ "document", " pdf doc docx odt xls xlsx ods ppt pptx odp epub " },
	{ "code", " c h cpp hpp rs go py js ts tsx jsx java kt rb php cs swift sh lua zig " },
	{ "program", " so dll exe a o lib rlib dylib bin pak " },
	{ "text/data", " json csv txt log xml yaml yml toml md parquet " },
	{ NULL, NULL },
};

const char *type_category(const char *ext)
{
	char key[24];
	if (!*ext)
		return "no extension";
	snprintf(key, sizeof key, " %s ", ext);
	for (const struct category *c = categories; c->name; c++)
		if (strstr(c->exts, key))
			return c->name;
	return "";
}

void analyze_free(struct analysis *a)
{
	free(a->findings);
	free(a->types);
	memset(a, 0, sizeof *a);
}

#ifndef DISKMOON_OPTIONS_H
#define DISKMOON_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

enum format { FORMAT_AUTO, FORMAT_TXT, FORMAT_NCDU, FORMAT_CSV };

struct options {
	char *root;
	const char *from;
	const char *compare;
	const char *output;
	enum format format;
	int max_depth;
	uint64_t min_size;
	int top;
	int jobs;
	uint64_t dupes_min;
	bool one_fs;
	bool apparent;
	bool all_fs;
	bool no_files;
	bool ascii;
	bool quiet;
	bool interactive;
};

#endif

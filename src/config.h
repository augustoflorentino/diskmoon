#ifndef DISKMOON_CONFIG_H
#define DISKMOON_CONFIG_H

#define DISKMOON_VERSION "0.1.0"
#define REPORT_FORMAT 1

#define DEFAULT_TOP_FILES 50
#define EXPLAIN_LIST 10
#define DEFAULT_REPORT_PREFIX "diskmoon"

#define ARENA_CHUNK (1u << 20)
#define NODE_SPACE_MAX ((size_t)NODE_UNIT << 31)
#define NODE_SPACE_MIN ((size_t)1 << 28)
#define INODE_SHARDS 64
#define PROGRESS_INTERVAL_MS 200
#define PREFIX_MAX 65536
#define LONG_PATH 65536
#define FD_DEPTH_MAX 64
#define DENTS_BUFFER (64u << 10)
#define MAX_AUTO_JOBS 32
#define ROTATIONAL_JOBS 2
#define OUTPUT_BUFFER (1u << 20)
#define MMAP_THRESHOLD (128 << 10)

#define GLYPH_UTF8_BRANCH "├── "
#define GLYPH_UTF8_LAST "└── "
#define GLYPH_UTF8_PIPE "│   "
#define GLYPH_ASCII_BRANCH "|-- "
#define GLYPH_ASCII_LAST "`-- "
#define GLYPH_ASCII_PIPE "|   "
#define GLYPH_BLANK "    "

#define RULE "================================================================================"

#endif

#ifndef DISKMOON_LINKS_H
#define DISKMOON_LINKS_H

#include <stddef.h>
#include <stdint.h>

#include "node.h"

struct link_ref {
	uint64_t ino;
	node_ref dir;
	node_ref node;
};

void links_settle(struct link_ref *links, size_t count);

#endif

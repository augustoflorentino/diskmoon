#ifndef DISKMOON_INODES_H
#define DISKMOON_INODES_H

#include <stdbool.h>
#include <stdint.h>

void inodes_init(void);
bool inodes_seen(uint64_t dev, uint64_t ino);

#endif

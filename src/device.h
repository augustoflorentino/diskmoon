#ifndef DISKMOON_DEVICE_H
#define DISKMOON_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

bool device_rotational(uint64_t dev);
uint64_t device_size(uint64_t dev);

#endif

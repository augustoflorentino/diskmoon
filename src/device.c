#include <limits.h>
#include <stdio.h>
#include <sys/sysmacros.h>

#include "device.h"

static long long read_number(const char *path)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return -1;
	long long value = -1;
	if (fscanf(f, "%lld", &value) != 1)
		value = -1;
	fclose(f);
	return value;
}

bool device_rotational(uint64_t dev)
{
	char path[PATH_MAX];
	unsigned major = major(dev), minor = minor(dev);
	snprintf(path, sizeof path, "/sys/dev/block/%u:%u/queue/rotational", major, minor);
	long long flag = read_number(path);
	if (flag < 0) {
		snprintf(path, sizeof path, "/sys/dev/block/%u:%u/../queue/rotational", major, minor);
		flag = read_number(path);
	}
	return flag == 1;
}

uint64_t device_size(uint64_t dev)
{
	char path[PATH_MAX];
	snprintf(path, sizeof path, "/sys/dev/block/%u:%u/size", major(dev), minor(dev));
	long long sectors = read_number(path);
	return sectors > 0 ? (uint64_t)sectors * 512 : 0;
}

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "longpath.h"

#define STEP_FLAGS (O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC | O_PATH)

static size_t chunk_end(const char *path, size_t from, size_t len)
{
	size_t end = from + PATH_MAX - 1 < len ? from + PATH_MAX - 1 : len;
	if (end == len)
		return end;
	while (end > from && path[end] != '/')
		end--;
	return end;
}

static int step(int dirfd, const char *path, size_t from, size_t end, int flags)
{
	char part[PATH_MAX];
	memcpy(part, path + from, end - from);
	part[end - from] = '\0';
	int fd = openat(dirfd, part[0] ? part : ".", flags);
	if (dirfd != AT_FDCWD)
		close(dirfd);
	return fd;
}

int open_long(const char *path, int flags)
{
	size_t len = strlen(path);
	if (len < PATH_MAX)
		return open(path, flags);
	int fd = AT_FDCWD;
	for (size_t from = 0; from < len;) {
		size_t end = chunk_end(path, from, len);
		if (end == from) {
			errno = ENAMETOOLONG;
			return -1;
		}
		fd = step(fd, path, from, end, end == len ? flags : STEP_FLAGS);
		if (fd < 0)
			return -1;
		from = end;
		while (from < len && path[from] == '/' && from)
			from++;
	}
	return fd;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "inodes.h"

struct key {
	uint64_t dev;
	uint64_t ino;
};

struct shard {
	pthread_mutex_t lock;
	struct key *slots;
	size_t cap;
	size_t len;
};

static struct shard shards[INODE_SHARDS];

static uint64_t mix(uint64_t x)
{
	x ^= x >> 30;
	x *= 0xbf58476d1ce4e5b9ull;
	x ^= x >> 27;
	x *= 0x94d049bb133111ebull;
	return x ^ (x >> 31);
}

static uint64_t key_hash(uint64_t dev, uint64_t ino)
{
	return mix(ino ^ mix(dev));
}

static bool slot_put(struct key *slots, size_t cap, uint64_t dev, uint64_t ino)
{
	size_t i = (key_hash(dev, ino) / INODE_SHARDS) & (cap - 1);
	for (;; i = (i + 1) & (cap - 1)) {
		if (!slots[i].ino) {
			slots[i] = (struct key){ dev, ino };
			return false;
		}
		if (slots[i].ino == ino && slots[i].dev == dev)
			return true;
	}
}

static void shard_grow(struct shard *s)
{
	size_t cap = s->cap ? s->cap * 2 : 1024;
	struct key *slots = calloc(cap, sizeof *slots);
	if (!slots) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
	for (size_t i = 0; i < s->cap; i++)
		if (s->slots[i].ino)
			slot_put(slots, cap, s->slots[i].dev, s->slots[i].ino);
	free(s->slots);
	s->slots = slots;
	s->cap = cap;
}

void inodes_init(void)
{
	for (int i = 0; i < INODE_SHARDS; i++) {
		free(shards[i].slots);
		shards[i] = (struct shard){ .slots = NULL };
		pthread_mutex_init(&shards[i].lock, NULL);
	}
}

bool inodes_seen(uint64_t dev, uint64_t ino)
{
	struct shard *s = &shards[key_hash(dev, ino) % INODE_SHARDS];
	pthread_mutex_lock(&s->lock);
	if ((s->len + 1) * 2 > s->cap)
		shard_grow(s);
	bool seen = slot_put(s->slots, s->cap, dev, ino);
	s->len += !seen;
	pthread_mutex_unlock(&s->lock);
	return seen;
}

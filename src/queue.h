#ifndef DISKMOON_QUEUE_H
#define DISKMOON_QUEUE_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "node.h"

struct task {
	struct node *dir;
	uint64_t dev;
};

struct queue {
	pthread_mutex_t lock;
	pthread_cond_t changed;
	pthread_cond_t idle;
	struct task *items;
	size_t len;
	size_t cap;
	size_t pending;
	atomic_size_t waiting;
};

void queue_init(struct queue *q);
void queue_push(struct queue *q, struct task t);
bool queue_pop(struct queue *q, struct task *out);
void queue_finish(struct queue *q);
bool queue_hungry(struct queue *q);
bool queue_wait_idle(struct queue *q, int timeout_ms);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queue.h"

void queue_init(struct queue *q)
{
	memset(q, 0, sizeof *q);
	pthread_mutex_init(&q->lock, NULL);
	pthread_cond_init(&q->changed, NULL);
	pthread_cond_init(&q->idle, NULL);
}

static void grow(struct queue *q)
{
	q->cap = q->cap ? q->cap * 2 : 1024;
	q->items = realloc(q->items, q->cap * sizeof *q->items);
	if (!q->items) {
		fputs("diskmoon: out of memory\n", stderr);
		exit(2);
	}
}

void queue_push(struct queue *q, struct task t)
{
	pthread_mutex_lock(&q->lock);
	if (q->len == q->cap)
		grow(q);
	q->items[q->len++] = t;
	q->pending++;
	pthread_cond_signal(&q->changed);
	pthread_mutex_unlock(&q->lock);
}

bool queue_pop(struct queue *q, struct task *out)
{
	pthread_mutex_lock(&q->lock);
	while (!q->len && q->pending) {
		atomic_fetch_add_explicit(&q->waiting, 1, memory_order_relaxed);
		pthread_cond_wait(&q->changed, &q->lock);
		atomic_fetch_sub_explicit(&q->waiting, 1, memory_order_relaxed);
	}
	bool got = q->len > 0;
	if (got)
		*out = q->items[--q->len];
	pthread_mutex_unlock(&q->lock);
	return got;
}

void queue_finish(struct queue *q)
{
	pthread_mutex_lock(&q->lock);
	if (!--q->pending) {
		pthread_cond_broadcast(&q->changed);
		pthread_cond_broadcast(&q->idle);
	}
	pthread_mutex_unlock(&q->lock);
}

bool queue_hungry(struct queue *q)
{
	return atomic_load_explicit(&q->waiting, memory_order_relaxed) > 0;
}

bool queue_wait_idle(struct queue *q, int timeout_ms)
{
	struct timespec deadline;
	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_nsec += (long)timeout_ms * 1000000L;
	deadline.tv_sec += deadline.tv_nsec / 1000000000L;
	deadline.tv_nsec %= 1000000000L;
	pthread_mutex_lock(&q->lock);
	if (q->pending)
		pthread_cond_timedwait(&q->idle, &q->lock, &deadline);
	bool idle = !q->pending;
	pthread_mutex_unlock(&q->lock);
	return idle;
}

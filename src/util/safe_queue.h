#ifndef _SAFE_QUEUE_H_
#define _SAFE_QUEUE_H_

// Safe queue implementation adapted from https://gist.github.com/ryankurte/

#include <stdlib.h>
#include <pthread.h>

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    void** data;
    pthread_mutex_t *mutex;
} queue_t;

queue_t *queue_create();
void queue_init(queue_t *, size_t);
void queue_destroy(queue_t *);
queue_t *queue_init_create(size_t);

void *queue_read(queue_t *queue);
int queue_write(queue_t *queue, void* handle);

#endif

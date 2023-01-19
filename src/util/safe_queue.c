#include "safe_queue.h"

queue_t *queue_create(){
    queue_t *self = malloc(sizeof(queue_t));
    self->mutex = malloc(sizeof(pthread_mutex_t));
    return self;
}

void queue_init(queue_t *self, size_t size){
    *self = (queue_t){0, 0, size, NULL,};
}

void queue_destroy(queue_t *self){
    free(self->mutex);
    free(self);
}

queue_t *queue_init_create(size_t size){
    queue_t *self = queue_create();
    queue_init(self, size);
    return self;
}

void* queue_read(queue_t *self) {
    if (self->tail == self->head) {
        return NULL;
    }

    pthread_mutex_lock(self->mutex);
    void* handle = self->data[self->tail];
    self->data[self->tail] = NULL;
    self->tail = (self->tail + 1) % self->size;
    pthread_mutex_unlock(self->mutex);
    return handle;

}

int queue_write(queue_t *self, void* handle) {
    if (((self->head + 1) % self->size) == self->tail) {
        return -1;
    }
    
    pthread_mutex_lock(self->mutex);
    self->data[self->head] = handle;
    self->head = (self->head + 1) % self->size;
    pthread_mutex_unlock(self->mutex);
    return 0;
}

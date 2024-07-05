#include <stdlib.h>


// Regular queue implemented as a vector with a ring buffer. Reallocates when
// full, doubling in size.
struct Dequeue {
    void *buffer;
    size_t size;
    unsigned int len;
    unsigned int capacity;
    unsigned int head;
};

struct Dequeue dequeue_create(size_t size);
int dequeue_pop_tail(struct Dequeue *q, void *element);
void *dequeue_get(struct Dequeue *q, int i);
int dequeue_pop_head(struct Dequeue *q, void *element);
int dequeue_push_tail(struct Dequeue *q, void *element);
int dequeue_push_head(struct Dequeue *q, void *element);
int dequeue_grow(struct Dequeue *q, unsigned int n);
void dequeue_destroy(struct Dequeue *q);

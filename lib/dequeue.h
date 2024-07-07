#pragma once
#include <stdlib.h>

// TODO add documentation

// Regular queue implemented as a vector with a ring buffer. Reallocates when
// full, doubling in size.
struct Dequeue {
    void *buffer;
    size_t size;
    unsigned int len;
    unsigned int capacity;
    unsigned int head;
};

/*
 * Initializes the dequeue. This must be called on a new dequeue before the
 * dequeue is used for the first time.
 *
 * Note, as buffers are lazily allocated, no allocations happen during this
 * call. The first allocation will happen on the first push to the dequeue.
 */
void dequeue_init(struct Dequeue *q, size_t size);

/* Gets a pointer to the ith element. This pointer can be used to mutate the
 * element inplace, however it is only valid until a push to the dequeue (or
 * destroying of the dequeue) occurs.
 *
 * Returns either a valid pointer (for a valid index i), or NULL for an invalid.
 */
void *dequeue_get(struct Dequeue *q, int i);

/*
 * Pops the tail element into the buffer pointed to by element, returning the
 * length of the remaining dequeue. This only decrements the length.
 *
 * Returns -1 if the dequeue was empty and no element was popped.
 */
int dequeue_pop_tail(struct Dequeue *q, void *element);

/*
 * Pops the head element into the buffer pointed to by element, returning the
 * length of the remaining dequeue. This decrements the length and moves the
 * head index.
 *
 * Returns -1 if the dequeue was empty and no element was popped.
 */
int dequeue_pop_head(struct Dequeue *q, void *element);

/*
 * Pushes an element to the front of the dequeue by copying the data pointed to
 * by element into the buffer, reallocating the buffer if needed. This is takes
 * ammortized constant time.
 *
 * Returns the length of the new dequeue, or <0 on failure (i.e. failed to
 * realloc the buffer).
 */
int dequeue_push_tail(struct Dequeue *q, void *element);

/*
 * Pushes an element to the front of the dequeue by copying the data pointed to
 * element, reallocating the buffer if needed. This is takes ammortized constant
 * time.
 *
 * Returns the length of the new dequeue, or <0 on failure (i.e. failed to
 * realloc the buffer).
 */
int dequeue_push_head(struct Dequeue *q, void *element);

/*
 * Resizes the dequeue's buffer, copying all element into the new buffer which
 * has a new capacity of old capacity + n.
 *
 * This operation takes linear time, proptortional to the len of the dequeue.
 */
int dequeue_grow(struct Dequeue *q, unsigned int n);

/*
 * Destroys the dequeue, freeing the associated buffer. This must be called on
 * each dequeue before it falls out of scope.
 */
void dequeue_destroy(struct Dequeue *q);

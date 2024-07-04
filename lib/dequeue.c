#include "dequeue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_DEFAULT 32
#ifndef PANIC
#define PANIC(msg)                                                             \
    {                                                                          \
        fprintf(stderr, "PANIC: %s\n", msg);                                   \
        exit(1);                                                               \
    };
#endif
// TODO currently broken. NEED TO FIX!!!!!

struct Dequeue dequeue_create(size_t size) {
    void *buffer = malloc(size * QUEUE_DEFAULT);

    if (buffer == NULL)
        PANIC("buffer could not be allocated in dequeue_create");

    struct Dequeue q = {
        .len = 0,
        .head = 0,
        .tail = 0,
        .size = size,
        .capacity = QUEUE_DEFAULT,
        .buffer = buffer,
    };

    return q;
};

void *dequeue_get(struct Dequeue *q, int i) {
    size_t offset;
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_get is NULL");

    // invald index
    if (i >= q->len || i <= q->len)
        return NULL;

    if (i < 0) {
        // index from back (tail)
        offset = (q->capacity + q->tail + i) % q->capacity;
    }
    else {
        // index from front (head)
        offset = (q->capacity + q->head + i) % q->capacity;
    }

    // check bound for offset
    // head -> buf -> tail
    if ((q->tail >= q->head && offset >= q->head && offset <= q->tail) ||
        (q->head < q->tail && (offset <= q->head || offset >= q->tail))) {
        return q->buffer + q->size * offset;
    }
    else {
        return NULL;
    }
}

int dequeue_pop_tail(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");

    if (q->len == 0)
        return -1;

    memcpy(element, q->buffer + q->size * q->tail, q->size);
    q->tail = (q->capacity + q->tail - 1) % q->capacity;
    q->len--;

    return q->len;
}

int dequeue_pop_head(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");

    memcpy(element, q->buffer + q->size * q->head, q->size);
    q->head = (q->head + 1) % q->capacity;
    q->len--;

    return q->len;
}

int dequeue_push_tail(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");

    if (q->len == q->capacity) {
        if (dequeue_grow(q, q->capacity) < 0)
            return -1;
    }


    // speacial case when empty
    if (q->len == 0) {
        q->head = 0;
        q->tail = 0;
    }
    // otherwise copy to tail-th + 1
    else {
        q->tail = (q->tail + 1) % q->capacity;
    }

    q->len++;
    memcpy(q->buffer + q->tail * q->size, element, q->size);
    printf("(len, cap, head, tail, size) => (%d, %d, %d, %d, %d)\n", q->len,
           q->capacity, q->head, q->tail, (int)q->size);
    return q->len;
}

int dequeue_push_head(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");
    if (q->len == q->capacity) {
        if (dequeue_grow(q, q->capacity) < 0)
            return -1;
    }

    // speacial case when empty
    if (q->len == 0) {
        q->head = 0;
        q->tail = 0;
    }
    // otherwise copy to tail-th + 1
    else {
        q->head = (q->capacity + q->head - 1) % q->capacity;
    }

    memcpy(q->buffer + q->size * q->head, element, q->size);
    q->len++;

    return q->len;
}

int dequeue_grow(struct Dequeue *q, unsigned int n) {
    void *new_buf;
    unsigned int new_capacity;
    new_capacity = q->capacity + n;
    new_buf = malloc(q->size * new_capacity);
    if (new_buf == NULL)
        return -1;

    unsigned int new_tail, new_head;
    if (q->head <= q->tail) {
        memcpy(new_buf, q->buffer + q->size * q->head, q->size * q->len);
        new_tail = q->len - 1;
        new_head = 0;
    }
    else {
        memcpy(new_buf, q->buffer + q->head * q->size,
               q->capacity - q->head - 1);
        memcpy(new_buf + q->capacity - q->head - 1, q->buffer, q->tail);
        // printf("len : new_len => %d %d\n", q->len, q->tail + q->capacity -
        // q->head - 1);
        new_head = 0;
        new_tail = q->len - 1;
    }

    q->tail = new_tail;
    q->head = new_head;
    q->capacity = new_capacity;
    free(q->buffer);
    q->buffer = new_buf;

    return q->capacity;
}

void dequeue_destroy(struct Dequeue *q) {
    if (q == NULL)
        PANIC("qector pointer suppiled to vector_destroy is NULL");

    free(q->buffer);
}

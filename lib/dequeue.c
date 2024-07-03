#include "dequeue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// TODO currently broken. NEED TO FIX!!!!!

struct Dequeue dequeue_create(size_t size) {
    void **b = malloc(sizeof(QUEUE_DEFAULT * size));

    if (b == NULL)
        PANIC("buffer could not be allocated in dequeue_create");

    struct Dequeue q = {
        .len = 0,
        .head = 0,
        .tail = 0,
        .size = size,
        .capacity = QUEUE_DEFAULT,
        .buffer = b,
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
    printf("q cap: %d\n", q->capacity);
    q->head = (q->head + 1) % q->capacity;
    q->len--;

    return q->len;
}

int dequeue_push_tail(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");
    // TODO make this 1 line
    if (q->len == q->capacity) {
        if (dequeue_grow(q, q->capacity) < 0)
            return -1;
    }

    q->tail = (q->capacity + q->tail + 1) % q->capacity;
    memcpy(q->buffer + q->size * q->tail, element, q->size);
    q->len++;

    printf("q cap: %d\n", q->capacity);
    return q->len;
}

int dequeue_push_head(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");
    if (q->len == q->capacity) {
        if (dequeue_grow(q, q->capacity) < 0)
            return -1;
    }

    q->head = (q->head + 1) % q->capacity;
    memcpy(q->buffer + q->size * q->head, element, q->size);
    q->len++;

    printf("q cap: %d\n", q->capacity);
    return q->len;
}

int dequeue_grow(struct Dequeue *q, unsigned int n) {
    void **new_buf;
    unsigned int new_capacity;
    new_capacity = q->capacity + n;
    new_buf = malloc(q->size * new_capacity);
    if (new_buf == NULL)
        return -1;

    unsigned int new_tail, new_head;
    if (q->head > q->tail) {
        memcpy(q->buffer + q->size * q->tail, new_buf + q->size * q->tail,
               q->size * (q->head - q->tail));
        new_tail = q->tail;
        new_head = q->head;
    }
    else {
        new_head = q->head;
        new_tail = new_capacity - q->tail - 1;
        memcpy(q->buffer, new_buf, new_head * q->size);
        memcpy(q->buffer + q->size * q->tail, new_buf + q->size * new_tail,
               new_capacity - new_tail - 1);
    }

    q->tail = new_tail;
    q->head = new_head;
    q->capacity = new_capacity;
    q->buffer = new_buf;

    return q->capacity;
}

void dequeue_destroy(struct Dequeue *q, int free_elements) {
    if (q == NULL)
        PANIC("qector pointer suppiled to vector_destroy is NULL");

    if (free_elements) {
        for (int i = 0; i < q->len; i++) {
            free(*(q->buffer + q->size * i));
        }
    }

    free(q->buffer);
}

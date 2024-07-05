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

#define INDEX(head, cap, n) (((cap) + (head) + (n)) % (cap))
// TODO currently broken. NEED TO FIX!!!!!

struct Dequeue dequeue_create(size_t size) {
    void *buffer = malloc(size * QUEUE_DEFAULT);

    if (buffer == NULL)
        PANIC("buffer could not be allocated in dequeue_create");

    struct Dequeue q = {
        .len = 0,
        .head = 0,
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
    if (i < -1 * (long)q->len || i >= (long)q->len) {
        // invalid index
        return NULL;
    }
    else if (i < 0) {
        // index from back
        return q->buffer + q->size * INDEX(q->head, q->capacity, q->len + i);
    }
    else {
        return q->buffer + q->size * INDEX(q->head, q->capacity, i);
    }

    // index from front (head)
    return q->buffer + q->size * INDEX(q->head, q->capacity, i);
}

int dequeue_pop_tail(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");

    if (q->len == 0)
        return -1;

    memcpy(element,
           q->buffer + q->size * INDEX(q->head, q->capacity, q->len - 1),
           q->size);
    q->len--;

    return q->len;
}

int dequeue_pop_head(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");
    // printf("Tail index = %d\n", INDEX(q->head, q->capacity, q->len - 1));
    // printf("Head index = %d\n", INDEX(q->head, q->capacity, 0));
    // for (int i = 0; i < q->capacity; i++) {
    //     int val = *((int *)q->buffer + i);
    //     if (val >= 0)
    //         printf("%d ", val);
    //     else
    //         printf(". ");
    // }
    // printf("\n");
    // for (int i = 0; i < q->len; i++) {
    //     if (i == q->head)
    //         printf("^ ");
    //     else if (i == INDEX(q->head, q->capacity, q->len - 1))
    //         printf("# ");
    //     else
    //         printf("  ");
    // }

    // printf("\n");

    memcpy(element, q->buffer + q->size * q->head, q->size);
    q->head = INDEX(q->head, q->capacity, 1);
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

    memcpy(q->buffer + q->size * INDEX(q->head, q->capacity, q->len), element,
           q->size);
    q->len++;
    return q->len;
}

int dequeue_push_head(struct Dequeue *q, void *element) {
    if (q == NULL)
        PANIC("queue pointer suppiled to queue_push is NULL");

    if (q->len == q->capacity) {
        if (dequeue_grow(q, q->capacity) < 0)
            return -1;
    }

    q->head = INDEX(q->head, q->capacity, -1);
    memcpy(q->buffer + q->size * q->head, element, q->size);
    q->len++;

    return q->len;
}

int dequeue_grow(struct Dequeue *q, unsigned int n) {
    unsigned int new_capacity = q->capacity + n;
    void *new_buf = malloc(q->size * new_capacity);

    if (new_buf == NULL)
        return -1;

    unsigned int tail = INDEX(q->head, q->capacity, q->len - 1);
    if (q->head <= tail) {
        memcpy(new_buf, q->buffer + q->size * q->head, q->size * q->len);
    }
    else {
        unsigned int right_len = q->capacity - q->head;
        unsigned int left_len = q->len - right_len;

        memcpy(new_buf, q->buffer + q->head * q->size, q->size * right_len);
        memcpy(new_buf + right_len * q->size, q->buffer, q->size * left_len);
    }

    q->head = 0;
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

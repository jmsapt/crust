#include "./vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_DEFAULT 64

#ifndef PANIC
#define PANIC(msg)                                                             \
    {                                                                          \
        fprintf(stderr, "PANIC: %s\n", msg);                                   \
        exit(1);                                                               \
    };
#endif

struct Vector vector_create(size_t size) {
    void *buffer = malloc(size * VECTOR_DEFAULT);
    if (buffer == NULL)
        PANIC("buffer could not be allocated in vector_create");
    struct Vector v;

    v.buffer = buffer;
    v.size = size;
    v.len = 0;
    v.capacity = VECTOR_DEFAULT;

    return v;
}

int vector_grow(struct Vector *v, unsigned int n) {
    if (v == NULL)
        PANIC("vector pointer suppiled to vector_grow is NULL");

    void *new_buf = realloc(v->buffer, v->size * (v->capacity + n));
    if (new_buf == NULL)
        return -1;

    v->capacity = v->capacity + n;
    v->buffer = new_buf;
    return v->capacity;
}

/* Copies element pointed to by element into the vector. In rust land this would
 * be treating element like a copy type.
 */
int vector_push(struct Vector *v, void *element) {
    if (v == NULL)
        PANIC("vector pointer suppiled to vector_push is NULL");
    if (element == NULL)
        PANIC("element pointer suppiled to vector_push is NULL");

    if (v->len == v->capacity) {
        if (vector_grow(v, v->len) < 0)
            return -1;
    }

    memcpy(v->buffer + v->len * v->size, element, v->size);
    v->len++;
    return v->len;
}

int vector_pop(struct Vector *v, void *element) {
    if (v == NULL)
        PANIC("vector pointer suppiled to vector_pop is NULL");
    if (element == NULL)
        PANIC("element pointer suppiled to vector_pop is NULL");

    if (v->len == 0)
        return -1;

    memcpy(element, v->buffer + (v->len - 1) * v->size, v->size);
    v->len--;
    return v->len;
}

void *vector_get(struct Vector *v, int i) {
    if (v == NULL)
        PANIC("vector pointer suppiled to vector_pop is NULL");

    if (i < -1 * (int)v->len || i >= (int)v->len) {
        // invalid index
        return NULL;
    }
    else if (i < 0) {
        // index from back
        return v->buffer + v->size * (v->len + i);
    }
    else {
        return v->buffer + v->size * i;
    }
}
/*
 * Destroys a vector, freeing all associated memory. Free elements if
 * free_elements is 1, otherwise leaves those pointers untouched.
 *
 * Panics if given NULL pointer to vector.
 */
void vector_destroy(struct Vector *v) {
    if (v == NULL)
        PANIC("vector pointer suppiled to vector_destroy is NULL");
    free(v->buffer);
}

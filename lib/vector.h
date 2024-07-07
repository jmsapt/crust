#pragma once
#include <stdlib.h>

/* Contigous buffer that is reallocated when len == capacity.
 *
 * Default capacity is 64 elements
 */
struct Vector {
    void *buffer;
    size_t size;
    unsigned int len;
    unsigned int capacity;
};

/*
 * Creates and allocates a new vector, returning NULL on failure.
 */
void vector_init(struct Vector *v, size_t size);

/*
 * Grows vector by n elements.
 *
 * Returns new vector size, or -1 on failure.
 */
int vector_grow(struct Vector *v, unsigned int n);

/*
 * Pushes elements to back of vector. Copies mem pointed to by element into
 * vector, treating element like a copy type (if we were in Rust land).
 *
 * Return length of vector after pushing element, or -1 on failure.
 *
 * Panics if given a NULL pointer.
 */
int vector_push(struct Vector *v, void *element);

/*
 * Pops tail (Nth) element from supplied vector, returning popped element to the
 * memory pointed to by element.
 *
 * Returns size of remaining vector. If vector empty still returns 0, but does
 * not modify element.
 *
 * Panics if given a NULL vector.
 */
int vector_pop(struct Vector *v, void *element);

/* Returns the pointer to the ith element from the head of the vector, or the
 * ith element from the tail (similarily to python) when provided a negative
 * index. Note that -1 indexes the tail.
 *
 * SAFETY: the return pointer must be considered invalid if the buffer is
 * reallocated either by growing or pushing to the vector. The pointer is also
 * invalidated by destroying the vector.
 *
 * Null is returned on attempting to index an invalid element, otherwise the
 * pointer to the element is returned. This pointer can be used to mutate the
 * element inplace.
 */
void *vector_get(struct Vector *v, int i);

/*
 * Destroys a vector, freeing all associated memory. Free elements if
 * free_elements is 1, otherwise leaves those pointers untouched.
 *
 * Panics if given NULL pointer to vector.
 */
void vector_destroy(struct Vector *v);

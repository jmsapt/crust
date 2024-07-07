#pragma once
#include <stdlib.h>

#include "vector.h"

struct HashMap {
    // the buffer that contains the head pointer to each list of buckets
    struct Bucket **bucket_buf;
    size_t key_size;
    size_t val_size;
    unsigned int len;
    unsigned int capacity;
    // functions
    int (*cmp_fn)(const void *a, const void *b);
    unsigned long (*hash_fn)(const void *);
};

struct Bucket {
    struct Bucket *next;
    // Allocated blocks for key and value
    void *val;
    void *key;
};

void hashmap_init(struct HashMap *m, const size_t key_size, const size_t val_size,
                              unsigned long (*hash_fn)(const void *),
                              int (*cmp_fn)(const void *a, const void *b));

void hashmap_destroy(struct HashMap *m);

/* Inserts key into the hashmap, placing the bucket at the index return by the
 * hash function. Insertion is done by copying the value pointed to by both the
 * key and val pointers into the map.
 *
 * If the key already exists, then the value is overwritten and replace with the
 * new supplied value. This replacement is done inplace, so previous pointers to
 * this element will remain valid, and pointing to the new inserted value.
 *
 * Returns 0 on success, and <0 on error.
 */
int hashmap_insert(struct HashMap *m, const void *key, const void *val);

/*
 * Removes the corresponding key-value pair from the map. If key does not
 * exists, then this has not effect on the map.
 */
void hashmap_erase(struct HashMap *m, const void *key);

/*
 * Returns a malloc-ed buffer filled with `len` number of keys. this buffer must
 * be freed before it falls out of scope.
 *
 * These keys are copied out of the map, so they will/can outlive the hashmap
 * itself.
 */
void *hashmap_keys(const struct HashMap *m);

/* Returns the pointer to the value associated with the provided key. This
 * pointer can be used to mutate the value inplace.
 *
 * It will also only live as long as the map itself.
 */
void *hashmap_get(const struct HashMap *m, const void *key);
/*
 * Given a key, returns 1 if the key is contained in the map, or 0 if it is not
 * contained in the map.
 */
int hashmap_contains(const struct HashMap *m, const void *key);

/* ----- Provided Hash Functions ----- */
unsigned long hash_int(const void *key);
unsigned long hash_long(const void *key);
unsigned long hash_string(const void *key);

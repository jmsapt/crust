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

/* Initialises the hashmap given the sizes of the key/value, and associated
 * hashing and comparision functions.
 *
 * Note that as the buffer is lazily allocated, there are no allocations that
 * occur during the call.
 */
void hashmap_init(struct HashMap *m, const size_t key_size,
                  const size_t val_size, unsigned long (*hash_fn)(const void *),
                  int (*cmp_fn)(const void *a, const void *b));

/*
 * Destroys the hashmap, free all associated memory. Note, is user malloced
 * memory is used in the keys or values, these allocations must be free by the
 * user prior to calling destroy. This can be achieved by collecting the
 * keys/values into a buffer then freeing them from that buffer.
 *
 * This function call takes linear time proportional to the capacity plus length
 * of the map.
 */
void hashmap_destroy(struct HashMap *m);

/*
 * Inserts key into the hashmap, placing the bucket at the index return by the
 * hash function. Insertion is done by copying the value pointed to by both the
 * key and val pointers into the map.
 *
 * If the key already exists, then the value is overwritten and replace with the
 * new supplied value. This replacement is done inplace, so previous pointers to
 * this element will remain valid, and pointing to the new inserted value.
 *
 * This function call takes amortized constant time.
 *
 * Returns 0 on success, and <0 on error.
 */
int hashmap_insert(struct HashMap *m, const void *key, const void *val);

/*
 * Removes the corresponding key-value pair from the map. If key does not
 * exists, then this has not effect on the map.
 *
 * This function call takes amortized constant time.
 */
void hashmap_erase(struct HashMap *m, const void *key);

/*
 * Copies keys and values into the respective buffers. These buffer must be at
 * least the same length as the hashmap.
 *
 * If a NULL pointer is supplied to either key_buf or val_buf, then that buffer
 * is skipped and no values are copied. For example, if key_buf is supplied but
 * val_buf is NULL, only the keys are copied. If both are valid, both are
 * copied, if neither is valid neither is copied.
 *
 * This function call takes linear time proportional to the map's length.
 */
void hashmap_key_val(const struct HashMap *m, void *key_buf, void *val_buf);

/*
 * Returns the pointer to the value associated with the provided key. This
 * pointer can be used to mutate the value inplace. It will also only live as
 * long as the map itself.
 *
 * Returns either a pointer to the element, or NULL if the element doesn't
 * exist.
 *
 * This function call takes amortized constant time.
 */
void *hashmap_get(const struct HashMap *m, const void *key);

/*
 * Given a key, returns 1 if the key is contained in the map, or 0 if it is not
 * contained in the map.
 *
 * This function call takes amortized constant time.
 */
int hashmap_contains(const struct HashMap *m, const void *key);

/* ----- Provided Hash Functions ----- */
unsigned long hash_int(const void *key);
unsigned long hash_long(const void *key);
unsigned long hash_string(const void *key);

/* ----- Provided Comparison Functions ----- */
// TODO

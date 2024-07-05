#include "hashmap.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Capacity factor to prompt reallocation of the
// buffer (and rehashing)
#define MAX_LOAD_FACTOR 0.7

// Default initial buffer size
#define HASHMAP_DEFAULT 256

#define MAGIC_NUM 0x45d9f3b

#ifndef PANIC
#define NULL_CHECK(ptr, msg)                                                   \
    if ((ptr) == NULL)                                                         \
        PANIC(msg);

#define PANIC(msg)                                                             \
    {                                                                          \
        fprintf(stderr, "PANIC: %s\n", msg);                                   \
        exit(1);                                                               \
    };
#endif

// Index in the buffer of bucket head pointers

typedef struct HashMap HashMap;
typedef struct Bucket Bucket;
typedef struct Vector Vector;

inline static Bucket *get_bucket(HashMap *m, void *key);
inline static unsigned long buffer_index(unsigned long (*hash_fn)(const void *),
                                         unsigned long capacity,
                                         const void *key);
inline static int expand_buffer(HashMap *m, unsigned long n);
inline static int buffer_insert(Bucket **buffer, Bucket *bucket,
                                int (*cmp_fn)(const void *a, const void *b),
                                unsigned long i, size_t v_size);

/* ------ Static helper functions ------ */
inline static int buffer_insert(Bucket **buffer, Bucket *bucket,
                                int (*cmp_fn)(const void *a, const void *b),
                                unsigned long i, size_t v_size) {
    NULL_CHECK(bucket, "Bucket cannot be null");

    Bucket *prev, *curr = buffer[i];
    if (curr == NULL) {
        buffer[i] = bucket;
        bucket->next = NULL;
        return 1;
    }

    // find tail
    while (curr != NULL) {
        if (cmp_fn(curr->key, bucket->key) == 0) {
            memcpy(curr->val, bucket->val, v_size);

            // discard bucket
            // TODO refactor to eleminate this allocation, only allocating when
            // neeeded (i.e. here)
            free(bucket->key);
            free(bucket->val);
            free(bucket);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    prev->next = bucket;
    bucket->next = NULL;
    return 1;
}

inline static int expand_buffer(HashMap *m, unsigned long n) {
    unsigned long new_cap = m->capacity + n;
    Bucket **new_buf = malloc(sizeof(Bucket *) * new_cap);
    if (new_buf == NULL)
        return -1;

    for (int i = 0; i < new_cap; i++)
        new_buf[i] = NULL;

    // rehash and reinsert each element, iterating over old buffer
    for (int i = 0; i < m->capacity; i++) {
        Bucket *prev, *curr = m->bucket_buf[i];

        while (curr != NULL) {
            // for each node in the bucket linked list,
            // insert into new buffer
            prev = curr;
            curr = curr->next;
            unsigned long new_i = buffer_index(m->hash_fn, new_cap, prev->key);
            buffer_insert(new_buf, prev, m->cmp_fn, new_i, m->val_size);
        }
    }

    // free old buffer
    free(m->bucket_buf);
    m->bucket_buf = new_buf;
    m->capacity = new_cap;

    return 0;
}

inline static unsigned long buffer_index(unsigned long (*hash_fn)(const void *),
                                         unsigned long capacity,
                                         const void *key) {
    return hash_fn(key) % capacity;
}

/* Gets the raw bucket for a given key.
 */
inline static Bucket *get_bucket(HashMap *m, void *key) {
    // expand buffer if needed
    double load = (double)m->len / m->capacity;
    if (load >= MAX_LOAD_FACTOR)
        expand_buffer(m, m->capacity);

    Bucket *curr = m->bucket_buf[buffer_index(m->hash_fn, m->capacity, key)];
    while (curr) {
        if (m->cmp_fn(curr->key, key) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

/* ------ Non-static definitions ------ */

struct HashMap hashmap_create(const size_t key_size, const size_t val_size,
                              unsigned long (*hash_fn)(const void *),
                              int (*cmp_fn)(const void *a, const void *b)) {
    Bucket **buffer = malloc(sizeof(Bucket *) * HASHMAP_DEFAULT);
    NULL_CHECK(buffer, "Buffer for hashmap could not be allocated");

    for (int i = 0; i < HASHMAP_DEFAULT; i++)
        buffer[i] = NULL;

    HashMap m = {
        .bucket_buf = buffer,
        .len = 0,
        .capacity = HASHMAP_DEFAULT,
        .key_size = key_size,
        .val_size = val_size,
        .hash_fn = hash_fn,
        .cmp_fn = cmp_fn,
    };

    return m;
}

void hashmap_destroy(struct HashMap *m) {
    for (int i = 0; i < m->capacity; i++) {
        // free linked-list of buckets
        if (m->bucket_buf[i] == NULL)
            continue;

        Bucket *curr, *prev;
        curr = m->bucket_buf[i];
        prev = NULL;
        while (curr != NULL) {
            prev = curr;
            curr = curr->next;

            free(prev->key);
            free(prev->val);
            free(prev);
        }

        // // free tail
        // free(prev->key);
        // free(prev->val);
        // free(prev);
    }

    free(m->bucket_buf);
}

/* Inserts key into the hashmap, placing the bucket at the index return by the
 * hash function. Insertion is done by copying the value pointed to by both the
 * key and val pointers into the map.
 *
 * If the key already exists, then the value is overwritten and replace with the
 * new supplied value.
 *
 * Returns 0 on success, and <0 on error.
 */
int hashmap_insert(struct HashMap *m, const void *key, const void *val) {
    double load = ((double)m->len) / m->capacity;
    if (load >= MAX_LOAD_FACTOR)
        expand_buffer(m, m->capacity);

    // Create new bucket, N
    Bucket *new_bucket = malloc(sizeof(Bucket));

    new_bucket->key = malloc(m->key_size);
    new_bucket->val = malloc(m->val_size);
    new_bucket->next = NULL;

    memcpy(new_bucket->key, key, m->key_size);
    memcpy(new_bucket->val, val, m->val_size);

    unsigned long i = buffer_index(m->hash_fn, m->capacity, key);
    m->len +=
        buffer_insert(m->bucket_buf, new_bucket, m->cmp_fn, i, m->val_size);

    return 0;
}

/*
 * Removes the corresponding key-value pair from the map. If key does not
 * exists, then this has not effect on the map.
 */
void hashmap_erase(struct HashMap *m, const void *key) {
    unsigned long i = buffer_index(m->hash_fn, m->capacity, key);
    Bucket *prev = NULL;
    Bucket *curr = m->bucket_buf[i];
    while (curr != NULL) {
        if (m->cmp_fn(curr->key, key) == 0) {
            // link prev to next nodes
            if (prev == NULL) {
                // head case
                m->bucket_buf[i] = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            // free current node
            free(curr->key);
            free(curr->val);
            free(curr);

            // decrement count
            m->len--;
            return;
        }

        curr = curr->next;
    }
}

/*
 * Returns a malloc-ed buffer filled with `len` number of keys. This buffer must
 * be freed before it falls out of scope.
 */
void *hashmap_keys(const HashMap *m) {
    //
    // TODO
    //
    return NULL;
}

/*
 * Given a key, returns 1 if the key is contained in the map, or 0 if it is not
 * contained in the map.
 */
int hashmap_contains(const struct HashMap *m, const void *key) {
    // check linked list for node
    unsigned long i = buffer_index(m->hash_fn, m->capacity, key);

    Bucket *curr = m->bucket_buf[i];
    while (curr) {
        if (m->cmp_fn(curr->key, key) == 0)
            return 1;

        curr = curr->next;
    }

    // key not found
    return 0;
}

/* ----- Provided Hash Functions ----- */
/* Please note that these hash functions have not really been verified and are
 * very likely highly scuffed. If possible opt to use your own hash function
 * that is know to be realiable.
 */
// TODO write standard hash functions
// TODO find more reliable hash fn
unsigned long hash_int(const void *key) {
    // from this stackoverflow answer
    // <https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key>

    // bitcast straight into a long, is this valid or reliable?
    // Who knows, cause I do not
    unsigned long x = *((unsigned int *)key);

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    return x;
}

unsigned long hash_long(const void *key) {
    // from this stackoverflow answer
    // <https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key>

    // bitcast straight into a long, is this valid or reliable?
    // Who knows, cause I do not
    unsigned long x = *((unsigned long *)key);

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    return x;
}

unsigned long hash_string(const void *key) {
    // sdbm hasing algorithm
    // <http://www.cse.yorku.ca/~oz/hash.html>
    const unsigned char *str = *(const unsigned char **)key;
    unsigned long hash = 0;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

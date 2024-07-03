#include "../lib/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RAND_SEED 0xABCDEF
#define MAX_BUFFER 8192
// Be careful going above 20
#define SAMPLE_SIZE 22

typedef struct Vector Vector;

int test_push_pop(int len);
int test_grow(const int len);
int test_get(const int len);

int main(void) {
    int lens[] = {8, 64, 4096, MAX_BUFFER};

    // test correctness
    for (int i = 0; i < sizeof(lens) / sizeof(int); i++) {
        int len = lens[i];
        printf("Length %d\n", len);

        if (test_push_pop(len))
            return -1;
        else
            printf("- Passed push-pop\n");

        if (test_grow(len))
            return -1;
        else
            printf("- Passed grow\n");

        if (test_get(len))
            return -1;
        else
            printf("- Passed get\n");

        putchar('\n');
    }

    printf("Validating time complexity\n");
    // test timecomplexity
    long size = 64;

    // nanoseconds
    double mean_times[SAMPLE_SIZE] = {0.0};
    long n_samples[SAMPLE_SIZE];
    srand(RAND_SEED);

    Vector vec = vector_create(sizeof(int));
    for (int i = 0; i < SAMPLE_SIZE; i++, size *= 2) {
        clock_t t = clock();
        for (int j = 0; j < size; j++) {
            int val = rand();
            vector_push(&vec, &val);
        }

        t = clock() - t;
        mean_times[i] = ((double)t * 1000000000) / (CLOCKS_PER_SEC * size);
        n_samples[i] = size;
    }
    vector_destroy(&vec);

    printf("Mean insertion times (nanoseconds):\n");
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        printf("%10ld => %3.4f\n", n_samples[i] , mean_times[i]);
    }

    return 0;
}

/* Test growing vector.
 */
int test_grow(const int len) {
    int buffer[MAX_BUFFER] = {-1};
    int grow_lens[] = {128, 47, 11};

    srand(RAND_SEED);
    // build vec
    Vector vec = vector_create(sizeof(int));
    for (int i = 0; i < len; i++) {
        int val = rand();
        vector_push(&vec, &val);
        buffer[i] = val;
    }

    // grow vec n times
    int capacity = vec.capacity;
    for (int i = 0; i < sizeof(grow_lens) / sizeof(int); i++) {
        int additional = grow_lens[i];
        capacity += additional;
        vector_grow(&vec, additional);

        if (vec.capacity != capacity) {
            fprintf(stderr, "Vector capacity is wrong: exp %d != act %d\n",
                    capacity, vec.capacity);
            vector_destroy(&vec);
            return -1;
        }
        if (vec.len != len) {
            fprintf(stderr, "Vector length is wrong: exp %d != act %d\n", len,
                    vec.len);
            vector_destroy(&vec);
            return -2;
        }
        if (memcmp(vec.buffer, buffer, vec.len * vec.size)) {
            fprintf(stderr, "Buffers are not equal\n");
            vector_destroy(&vec);
            return -3;
        }
    }

    vector_destroy(&vec);
    return 0;
}

/* Test getting from vector.
 */
int test_get(const int len) {
    int buffer[MAX_BUFFER] = {-1};
    int pos_indexes[] = {0, len / 2, len - 1};
    int neg_indexes[] = {-1, -len / 2, -len};
    int invalid_indexes[] = {len, -len - 1};

    srand(RAND_SEED);
    // build vec
    Vector vec = vector_create(sizeof(int));
    for (int i = 0; i < len; i++) {
        int val = rand();
        vector_push(&vec, &val);
        buffer[i] = val;
    }

    // get positive index
    int *val;
    for (int i = 0; i < sizeof(pos_indexes) / sizeof(int); i++) {
        int index = pos_indexes[i];
        val = vector_get(&vec, index);
        if (val == NULL) {
            printf("Unexpected NULL (in pos index)\n");
            return -4;
        }
        else if (*val != buffer[index]) {
            printf("exp != act => %d != %d\n", *val, buffer[index]);
            return -1;
        }
    }

    // get negative index
    for (int i = 0; i < sizeof(neg_indexes) / sizeof(int); i++) {
        int index = neg_indexes[i];
        val = vector_get(&vec, index);
        if (val == NULL) {
            printf("Unexpected NULL (in neg index)\n");
            return -4;
        }
        if (*val != buffer[len + index]) {
            printf("exp != act => %d != %d\n", *val, buffer[len + index]);
        }
    }

    // invalid indexes
    for (int i = 0; i < sizeof(invalid_indexes) / sizeof(int); i++) {
        val = vector_get(&vec, invalid_indexes[i]);
        if (val != NULL)
            return -3;
    }

    vector_destroy(&vec);
    return 0;
}

/* Test pushing and popping from the vector.
 *
 * Compiling with santizer enable (or
 * using valgrind) will help validate that create and destroy are free from
 * memory leaks.
 */
int test_push_pop(int len) {
    int buffer[MAX_BUFFER] = {-1};

    srand(RAND_SEED);
    Vector vec = vector_create(sizeof(int));

    // build vector/static buffer
    for (int i = 0; i < len; i++) {
        int val = rand();
        vector_push(&vec, &val);
        buffer[i] = val;
    }

    if (vec.len != len) {
        vector_destroy(&vec);
        fprintf(stderr, "Vector length is wrong: exp %d != act %d\n", len,
                vec.len);
        return -1;
    }

    // compare vector/static buffer
    if (memcmp(vec.buffer, buffer, vec.len * vec.size)) {
        vector_destroy(&vec);
        fprintf(stderr, "Buffers are not equal\n");
        return -2;
    }

    // compare vector pop
    int pop_val;
    for (int i = vec.len - 1; vector_pop(&vec, &pop_val) >= 0; i--) {
        if (pop_val != buffer[i]) {
            fprintf(stderr, "Failed push-pop test of size %d\n", len);
            fprintf(stderr, "Popped %d, expected %d\n", pop_val, buffer[i]);
            vector_destroy(&vec);
            return -3;
        }
    }

    vector_destroy(&vec);
    return 0;
}

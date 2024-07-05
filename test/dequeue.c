#include "../lib/dequeue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RAND_SEED 0xABCDEF
#define MAX_BUFFER 8192
// Be careful going above 20
#define SAMPLE_SIZE 22

typedef struct Dequeue Dequeue;

int test_push_pop(int len);
int test_grow(const int len);
int test_get(const int len);

int main(void) {
    int lens[] = {8, 64, 1024, 4096};

    // test correctness
    for (int i = 0; i < sizeof(lens) / sizeof(int); i++) {
        int len = lens[i];
        printf("Length %d\n", len);

        if (test_push_pop(len))
            return -1;
        else
            printf("- Passed push-pop\n");

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

    Dequeue deq = dequeue_create(sizeof(int));
    for (int i = 0; i < SAMPLE_SIZE; i++, size *= 2) {
        clock_t t = clock();
        for (int j = 0; j < size; j++) {
            int val = rand();
            dequeue_push_tail(&deq, &val);
        }

        t = clock() - t;
        mean_times[i] = ((double)t * 1000000000) / (CLOCKS_PER_SEC * size);
        n_samples[i] = size;
    }
    dequeue_destroy(&deq);

    printf("Mean insertion times (nanoseconds):\n");
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        printf("%10ld => %3.4f\n", n_samples[i] , mean_times[i]);
    }

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
    Dequeue vec = dequeue_create(sizeof(int));
    for (int i = 0; i < len; i++) {
        dequeue_push_tail(&vec, &i);
        buffer[i] = i;
    }

    // get positive index
    int *val;
    for (int i = 0; i < sizeof(pos_indexes) / sizeof(int); i++) {
        int index = pos_indexes[i];
        val = dequeue_get(&vec, index);
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
        val = dequeue_get(&vec, index);
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
        val = dequeue_get(&vec, invalid_indexes[i]);
        if (val != NULL)
            return -3;
    }

    dequeue_destroy(&vec);
    return 0;
}

/* Test pushing and popping from the dequeue.
 *
 * Compiling with santizer enable (or
 * using valgrind) will help validate that create and destroy are free from
 * memory leaks.
 */
int test_push_pop(int len) {
    int buffer[MAX_BUFFER] = {-1};

    srand(RAND_SEED);
    Dequeue deq1 = dequeue_create(sizeof(int));
    Dequeue deq2 = dequeue_create(sizeof(int));

    // build dequeue/static buffer
    for (int i = 0; i < len; i++) {
        int val = rand();

        dequeue_push_tail(&deq1, &i);
        dequeue_push_head(&deq2, &i);
        buffer[i] = i;
    }

    if (deq1.len != len) {
        dequeue_destroy(&deq1);
        dequeue_destroy(&deq2);
        fprintf(stderr, "dequeue length is wrong: exp %d != act %d\n", len,
                deq1.len);
        return -1;
    }

    int pop_val;
    for (int i = 0; i < len; i++) {
        dequeue_pop_head(&deq1, &pop_val);
        if (pop_val != buffer[i]) {
            fprintf(stderr, "Failed pop-head test of size %d (i = %d)\n", len,
                    i);
            fprintf(stderr, "Popped %d, expected %d\n", pop_val, buffer[i]);
            dequeue_destroy(&deq1);
            dequeue_destroy(&deq2);
            return -3;
        }

        dequeue_pop_tail(&deq2, &pop_val);
        if (pop_val != buffer[i]) {
            fprintf(stderr, "Failed pop-tail test of size %d (i = %d)\n", len,
                    i);
            fprintf(stderr, "Popped %d, expected %d\n", pop_val, buffer[i]);
            dequeue_destroy(&deq1);
            dequeue_destroy(&deq2);
            return -4;
        }
    }

    dequeue_destroy(&deq2);
    dequeue_destroy(&deq1);
    Dequeue deq = dequeue_create(sizeof(int));

    for (int i = len / 2; i < len; i++) {
        dequeue_push_tail(&deq, &i);
        buffer[i] = i;
    }
    for (int i = len / 2 - 1; i >= 0; i--) {
        dequeue_push_head(&deq, &i);
        buffer[i] = i;
    }

    if (deq.len != len) {
        dequeue_destroy(&deq);
        fprintf(stderr, "dequeue length is wrong: exp %d != act %d\n", len,
                deq1.len);
        return -1;
    }
    for (int i = 0; i < len; i++) {
        dequeue_pop_head(&deq, &pop_val);
        if (pop_val != buffer[i]) {
            fprintf(stderr,
                    "Failed add to both ends test of size %d (i = %d)\n", len,
                    i);
            fprintf(stderr, "Popped %d, expected %d\n", pop_val, buffer[i]);
            dequeue_destroy(&deq);
            return -5;
        }
    }
    dequeue_destroy(&deq);
    return 0;
}

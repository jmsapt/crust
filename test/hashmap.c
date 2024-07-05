#include "../lib/hashmap.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
typedef struct HashMap HashMap;

int cmp_int(const void *a, const void *b);
int cmp_ulong(const void *a, const void *b);
int cmp_str(const void *a, const void *b);

void test_simple_create_destroy(void);
void test_simple_int_int(void);
void test_simple_ulong_int(void);
void test_simple_str_int(void);
void test_large_int_int(int n);

int main(void) {
    printf("Simple create and destroy...\n");
    test_simple_create_destroy();

    printf("Testing simple hashmap<ulong, int>...\n");
    test_simple_ulong_int();

    printf("Testing simple hashmap<int, int>...\n");
    test_simple_int_int();

    printf("Testing simple hashmap<str, int>...\n");
    test_simple_str_int();

    int sizes[] = {800, 4000, 16000, 32000, 100000, 400000, 1600000, 3200000};
    // time in ns
    double insert_times[100];
    int n = sizeof(sizes) / sizeof(sizes[0]);
    printf("\nTesting large numbers of insertions:\n");
    for (int i = 0; i < n; i++) {
        int x = sizes[i];
        printf("hashmap<int, int>, n = %d\n", x);
        clock_t t = clock();
        test_large_int_int(x);
        t = clock() - t;

        insert_times[i] =
            ((double)t) / (sizes[i] * CLOCKS_PER_SEC) * 1000000000;
    }

    printf("\nMean insertion times (nanoseconds) => insertions : time\n");
    for (int i = 0; i < n; i++)
        printf("%8.2f : %d\n", insert_times[i], sizes[i]);
}

void test_simple_create_destroy(void) {
    HashMap m = hashmap_create(sizeof(int), sizeof(int), hash_int, cmp_int);
    // do nothing
    hashmap_destroy(&m);
}

void test_simple_ulong_int(void) {
    unsigned long ulongs[] = {15, 69, 85, 60, 0, 0};
    HashMap m =
        hashmap_create(sizeof(unsigned long), sizeof(int), hash_int, cmp_ulong);
    int len = sizeof(ulongs) / sizeof(unsigned long);

    // insert
    for (int i = 0; i < len; i++) {
        hashmap_insert(&m, &ulongs[i], &i);
    }

    // check contains
    for (int i = 0; i < len; i++)
        if (hashmap_contains(&m, &ulongs[i]) == 0) {
            printf("Expected to find %ld (i = %d)\n", ulongs[i], i);
        }

    // check that length is n - 1
    if (m.len != len - 1) {
        printf("Incorrect map length exp => act : %d %u", len - 1, m.len);
    }

    // check values (not last value is duplicated, hence we have n-1 keys)
    for (int i = 0; i < len; i++)
        if (hashmap_contains(&m, &ulongs[i]) == 0)
            printf("Expected to find %ld (i = %d)\n", ulongs[i], i);

    // check erase
    for (int i = 0; i < len - 1; i++) {
        hashmap_erase(&m, &ulongs[i]);
        if (hashmap_contains(&m, &ulongs[i]) == 1)
            printf("Expected map to not contain %ld (i = %d)\n", ulongs[i], i);

        if (m.len != len - i - 2)
            printf("Expected map len to be %d, acutal %u (i = %d)\n",
                   len - i - 2, m.len, i);
    }

    if (m.len != 0)
        printf("Map had length %u, expected 0 (empty)", m.len);

    hashmap_destroy(&m);
}

void test_simple_int_int(void) {
    int ints[] = {-12,       15,        69,        85,  60, -11111,
                  123124124, 123123123, 123456789, -11, -11};
    HashMap m = hashmap_create(sizeof(int), sizeof(int), hash_int, cmp_int);
    int len = sizeof(ints) / sizeof(int);

    // insert
    for (int i = 0; i < len; i++) {
        hashmap_insert(&m, &ints[i], &i);
    }

    // check contains
    for (int i = 0; i < len; i++)
        if (hashmap_contains(&m, &ints[i]) == 0) {

            printf("Expected to find %d (i = %d)\n", ints[i], i);
        }

    // check that length is n - 1
    if (m.len != len - 1) {
        printf("Incorrect map length exp => act : %d %u", len - 1, m.len);
    }

    // check values (not last value is duplicated, hence we have n-1 keys)
    for (int i = 0; i < len; i++)
        if (hashmap_contains(&m, &ints[i]) == 0)
            printf("Expected to find %d (i = %d)\n", ints[i], i);

    // check erase
    for (int i = 0; i < len - 1; i++) {
        hashmap_erase(&m, &ints[i]);
        if (hashmap_contains(&m, &ints[i]) == 1)
            printf("Expected map to not contain %d (i = %d)\n", ints[i], i);

        if (m.len != len - i - 2)
            printf("Expected map len to be %d, acutal %u (i = %d)\n",
                   len - i - 2, m.len, i);
    }

    if (m.len != 0)
        printf("Map had length %u, expected 0 (empty)", m.len);

    hashmap_destroy(&m);
}

void test_large_int_int(int n) {
    HashMap m = hashmap_create(sizeof(int), sizeof(int), hash_int, cmp_int);

    for (int i = 0; i < n; i++)
        hashmap_insert(&m, &i, &i);

    if (m.len != n)
        printf("Expected map len to be %d, acutal %u\n", n, m.len);

    for (int i = 0; i < n; i++)
        hashmap_erase(&m, &i);

    if (m.len != 0)
        printf("Map had length %u, expected 0 (empty)", m.len);

    hashmap_destroy(&m);
}

void test_simple_str_int(void) {
    HashMap m =
        hashmap_create(sizeof(char *), sizeof(int), hash_string, cmp_str);
    // char *strs[] = {"the", "cat", "on", "the", "mat", "is", "flat"};
    char *strs[] = {"on\0", "the\0", "mat\0", "is\0", "flat\0", "fat\0"};
    int n = sizeof(strs) / sizeof(strs[0]);

    for (int i = 0; i < n; i++) {
        char **ptr = &strs[i];
        hashmap_insert(&m, ptr, &i);
    }

    for (int i = 0; i < n; i++)
        if (hashmap_contains(&m, &strs[i]) == 0)
            printf("Expected to contain: <%s>\n", strs[i]);

    if (m.len != n)
        printf("Expected map len to be %d, acutal %u\n", n, m.len);

    for (int i = 0; i < n; i++)
        hashmap_erase(&m, &strs[i]);

    if (m.len != 0)
        printf("Map had length %u, expected 0 (empty)", m.len);
    hashmap_destroy(&m);
}

/* ----- Compare functions ----- */
int cmp_int(const void *a, const void *b) {
    const int *x = a;
    const int *y = b;
    return *x - *y;
}

int cmp_ulong(const void *a, const void *b) {
    const ulong *x = a;
    const ulong *y = b;
    return *x - *y;
}

int cmp_str(const void *a, const void *b) {
    const char **x = (const char **)a;
    const char **y = (const char **)b;
    return strcmp(*x, *y);
}

#include <stdio.h>
#include <stdlib.h>
#define NUM_VALID_CHARS 53

#ifndef PANIC
#define PANIC(msg)                                                             \
    {                                                                          \
        fprintf(stderr, "PANIC: %s\n", msg);                                   \
        exit(1);                                                               \
    };
#endif

struct TryTree {
    struct TryTreeNode *root;
    unsigned int count;
};

struct TryTreeNode {
    struct TryTreeNode *nodes[NUM_VALID_CHARS];
    void *contents;
};

struct TryTree *trytree_create(void);
int trytree_insert(struct TryTree *t, char *str, void *element);
void *trytree_get(struct TryTree *t, char *str);
void trytree_destroy(struct TryTree *t, int free_elements);

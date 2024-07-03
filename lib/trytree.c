#include "./trytree.h"
#include <stdio.h>
#include <time.h>

static void trytreenode_free(struct TryTreeNode *tn, int free_elements);
static int trytreenode_insert(struct TryTreeNode *tn, char *str, void *element);
void *trytreenode_get(struct TryTreeNode *tn, char *str);

/* Maps the provided character to index in the trytree.
 *
 * Valid chars are a-z, A-Z, 0-9, and "-".
 */
static int map_char(char c) {
    // lowercase letter
    if (c >= 'a' && c <= 'z')
        return c - 'a';

    // uppercase letter
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 26;

    // other characters
    switch (c) {
    case '-':
        return 53;

    default:
        return -1;
    }
}

struct TryTree *trytree_create(void) {
    struct TryTree *t = malloc(sizeof(struct TryTree));
    if (t == NULL)
        return NULL;

    struct TryTreeNode *r = malloc(sizeof(struct TryTreeNode));
    if (r == NULL) {
        free(t);
        return NULL;
    }

    for (int i = 0; i < 53; i++) 
        r->nodes[i] = NULL;
    r->contents = NULL;
    
    t->root = r;
    t->count = 0;

    return t;
}

int trytree_insert(struct TryTree *t, char *str, void *element) {
    int res = trytreenode_insert(t->root, str, element);

    if (res == 0) {
        t->count++;
        return t->count;
    }
    else {
        return res;
    }
}

static void trytreenode_free(struct TryTreeNode *tn, int free_elements) {
    for (int i = 0; i < 26; i++) {
        if (tn->nodes[i] != NULL)
            trytreenode_free(tn, free_elements);
    }

    if (free_elements)
        free(tn->contents);

    free(tn);
}

static int trytreenode_insert(struct TryTreeNode *tn, char *str,
                              void *element) {
    // insert here
    if (str[0] == '\0') {
        tn->contents = element;
        return 0;
    }

    // check for illegal characters
    int i = map_char(str[0]);
    if (i < 0)
        return -2;

    // allocate new node when needed
    if (tn->nodes[i] == NULL) {
        struct TryTreeNode *new_tn = malloc(sizeof(struct TryTreeNode));
        for (int i = 0; i < 53; i++) 
            new_tn->nodes[i] = NULL;
        new_tn->contents = NULL;

        if (new_tn == NULL)
            return -1;
        tn->nodes[i] = new_tn;
    }

    return trytreenode_insert(tn->nodes[i], &str[1], element);
}

void *trytreenode_get(struct TryTreeNode *tn, char *str) {
    // does not exist
    if (tn == NULL)
        return NULL;

    // found node, return curr contents
    if (str[0] == '\0')
        return tn->contents;

    // check valid char
    int i = map_char(str[0]);
    if (i < 0)
        return NULL;

    return trytreenode_get(tn->nodes[i], &str[1]);
}

void *trytree_get(struct TryTree *t, char *str) {
    return trytreenode_get(t->root, str);
}

void trytree_destroy(struct TryTree *t, int free_elements) {
    if (t == NULL)
        PANIC("Attempted to free NULL trytree in trytree_destroy");
    trytreenode_free(t->root, free_elements);

    free(t);
}

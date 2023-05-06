#ifndef INC_4_HASHTABLE_NODE_H
#define INC_4_HASHTABLE_NODE_H

#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

struct Node {
    char *key;
    char *value;
    Node *next_collision;
};

Node *node_create(char *key, char *value);

void destroy_node(Node *node);

void add_collision(Node *node, char *key, char *value);

#endif //INC_4_HASHTABLE_NODE_H

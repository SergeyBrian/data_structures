#include "node.h"

Node *node_create(char *key, char *value) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->key = (char *) calloc(sizeof(char), strlen(key) + 1);
    node->value = (char *) calloc(sizeof(char), strlen(value) + 1);
    node->next_collision = NULL;

    strcpy(node->key, key);
    strcpy(node->value, value);

    return node;
}

void add_collision(Node *node, char *key, char *value) {
    if (node->next_collision) {
        add_collision(node->next_collision, key, value);
        return;
    }
    node->next_collision = node_create(key,value);
}

void destroy_node(Node *node) {
    if (!node) return;

    if (node->next_collision) destroy_node(node->next_collision);

    free(node->key);
    free(node->value);
    free(node);
}

void destroy_single_node(Node *node) {
    if (!node) return;

    free(node->key);
    free(node->value);
    free(node);
}

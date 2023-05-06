#include "hashtable.h"

HashTable *hashtable_create(int capacity) {
    HashTable *table = (HashTable *) malloc(sizeof(HashTable));
    table->capacity = capacity;
    table->node_count = 0;
    table->nodes = (Node **) malloc(sizeof(Node *) * capacity);
    for (int i = 0; i < capacity; i++) {
        table->nodes[i] = NULL;
    }

    return table;
}

void resize(HashTable *table) {

}

void destroy_table(HashTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        destroy_node(table->nodes[i]);
    }

    free(table->nodes);
    free(table);
}

void insert_collision(Node *node, char *key, char *value) {
    if (strcmp(key, node->key) == 0) {
        node->value = realloc(node, sizeof(char) * strlen(value));
        strcpy(node->value, value);
        return;
    }

    if (node->next_collision != NULL) {
        insert_collision(node->next_collision, key, value);
        return;
    }
    node->next_collision = node_create(key, value);
}

void hash_table_insert(HashTable *table, char *key, char *value) {
    unsigned long index = adler32(key, strlen(key)) % table->capacity;

    if (table->node_count++ == table->capacity) {
        resize(table);
    }

    if (table->nodes[index] != NULL) {
        insert_collision(table->nodes[index], key, value);
        return;
    }

    table->nodes[index] = node_create(key, value);
}

char *find_collision(Node *node, char *key) {
    if (strcmp(node->key, key) != 0) return find_collision(node->next_collision, key);
    if (strcmp(node->key, key) == 0) return node->value;
    return NULL;
}

char *hash_table_find(HashTable *table, char *key) {
    unsigned long index = adler32(key, strlen(key)) % table->capacity;
    Node *node = table->nodes[index];

    if (!node || strcmp(node->key, key) != 0) {
        printf("Value with key %s not found\n", key);
        return NULL;
    }

    if (node->next_collision) {
        return find_collision(node, key);
    }

    return node->value;
}

void remove_collision(Node *node, char *key) {
    if (!node) return;

    if (node->next_collision) {
        if (strcmp(node->next_collision->key, key) == 0) {
            Node *next = node->next_collision->next_collision;
            destroy_single_node(node->next_collision);
            node->next_collision = next;
            return;
        }
        remove_collision(node->next_collision, key);
    }
}

void hash_table_remove(HashTable *table, char *key) {
    unsigned long index = adler32(key, strlen(key)) % table->capacity;
    Node *node = table->nodes[index];

    if (!node) return;
    if (strcmp(node->key, key) == 0) table->nodes[index] = node->next_collision;
    else remove_collision(node, key);
}

void print_collision(Node *node, int *depth) {
    if (!node) return;
    for (int i = 0; i <= *depth; i++) {
        printf(" * ");
    }
    printf("{\"%s\": \"%s\"}\n", node->key, node->value);
    (*depth)++;
    print_collision(node->next_collision, depth);
}

void hash_table_print(HashTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        int depth = 0;
        printf("[%d]\n", i);
        print_collision(table->nodes[i], &depth);
    }
}

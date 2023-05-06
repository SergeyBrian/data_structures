#ifndef INC_4_HASHTABLE_HASHTABLE_H
#define INC_4_HASHTABLE_HASHTABLE_H

#include "node.h"
#include "adler32.h"

typedef struct {
    Node **nodes;

    int node_count;
    int capacity;
} HashTable;

HashTable *hashtable_create(int capacity);

void destroy_table(HashTable *table);

void hash_table_insert(HashTable *table, char *key, char *value);

#endif //INC_4_HASHTABLE_HASHTABLE_H

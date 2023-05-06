#ifndef INC_4_HASHTABLE_HASHTABLE_H
#define INC_4_HASHTABLE_HASHTABLE_H

#include <stdio.h>

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

char *hash_table_find(HashTable *table, char *key);

#endif //INC_4_HASHTABLE_HASHTABLE_H

#include <stdio.h>

#include "hashtable.h"

#define DEFAULT_CAPACITY 16

int main() {
    HashTable *table = hashtable_create(DEFAULT_CAPACITY);
    hash_table_insert(table, "value_1", "idk");
    hash_table_insert(table, "value_2", "sdlkfj");
    hash_table_insert(table, "value_3", "fuck");
    hash_table_insert(table, "value_5", "sorry");
    return 0;
}

#include <stdio.h>

#include "hashtable.h"

#define DEFAULT_CAPACITY 16

int main() {
    HashTable *table = hashtable_create(DEFAULT_CAPACITY);
    for (int i = 0; i < 40; i++) {
        char key[100] = "";
        char value[100] = "";
        sprintf(key, "key_%d", i);
        sprintf(value, "value_%d", i);
        hash_table_insert(table, key, value);
    }
    hash_table_remove(table, "key_33");
    hash_table_remove(table, "key_15");
    hash_table_remove(table, "key_24");
    hash_table_remove(table, "key_6");
    hash_table_print(table);
//    printf("%s\n", hash_table_find(table, "key_3"));
    return 0;
}

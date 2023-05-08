#include <stdio.h>
#include <time.h>

#include "hashtable.h"

#define DEFAULT_CAPACITY 16

char *random_string(int len) {
    char *string = calloc(sizeof(char), len);

    for (int i = 0; i < len - 1; i++) {
        string[i] = (char) ('a' + rand() % 26);
    }

    return string;
}

int main() {
    srand(1);
    HashTable *table = hashtable_create(DEFAULT_CAPACITY);
    int test_size = 10000;
    char keys[10000][100];
    for (int i = 0; i < test_size; i++) {
        char *key = random_string(2 + rand() % 10);
        strcpy(keys[i], key);
        char value[100] = "";
        sprintf(value, "value_%d", i);
        hash_table_insert(&table, key, value);
    }
//    hash_table_remove(table, "key_33");
//    hash_table_remove(table, "key_15");
//    hash_table_remove(table, "key_6");
//    hash_table_print(table);
    printf("%f\n", hash_table_get_fill_factor(table));
    return 0;
}

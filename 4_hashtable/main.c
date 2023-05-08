#include <stdio.h>
#include <time.h>

#include "hashtable.h"

#define DEFAULT_CAPACITY 16

#define MAX_COMMAND 512

char *random_string(int len) {
    char *string = calloc(sizeof(char), len);

    for (int i = 0; i < len - 1; i++) {
        string[i] = (char) ('a' + rand() % 26);
    }

    return string;
}

void test(int size) {
    HashTable *table = hashtable_create(DEFAULT_CAPACITY);
    clock_t begin = clock();
    for (int i = 0; i < size; i++) {
        char *key = random_string(2 + rand() % 10);
        char value[100] = "";
        sprintf(value, "value_%d", i);
        hash_table_insert(&table, key, value);
    }
    clock_t end = clock();
    double insert_time = (double)(end - begin) / CLOCKS_PER_SEC;

    int resize_count = 0;
    int i = DEFAULT_CAPACITY;

    while (i <= table->capacity) {
        i *= 2;
        resize_count++;
    }

    printf("Insert time: %f\n", insert_time);
    printf("Collisions: %d\n", hash_table_get_collisions(table));
    printf("Fill factor: %f\n", hash_table_get_fill_factor(table));
    printf("Resize count: %d\n", resize_count);
    destroy_table(table);
}

void parse();

int main() {
//    test(1500000);
    HashTable *table = hashtable_create(DEFAULT_CAPACITY);
    char buf[MAX_COMMAND] = "";
    while (strcmp(buf, "end")) {
        fgets(buf, MAX_COMMAND, stdin);
        buf[strlen(buf) - 1] = 0;
        buf[3] = 0;
        char *args = &buf[4];
        if (strcmp(buf, "put") == 0) {
            char *key = strtok(args, ", ");
            char *value = strtok(NULL, ", ");
            hash_table_insert(&table, key, value);
        }
        if (strcmp(buf, "get") == 0) {
            char *value = hash_table_find(table, args);
            if (value) {
                printf("%s\n", value);
            }
        }
        if (strcmp(buf, "del") == 0) {
            hash_table_remove(table, args);
        }
        if (strcmp(buf, "tes") == 0) {
            test(atoi(args));
        }
        if (strcmp(buf, "pri") == 0) {
            hash_table_print(table);
        }
    }
    destroy_table(table);
    return 0;
}

#include "hashtable.h"

HashTable *hashtable_create(int capacity) {
    HashTable *table = (HashTable *) malloc(sizeof(HashTable));
    table->capacity = capacity;
    table->node_count = 0;
    table->nodes = (Node **) calloc(sizeof(Node *), capacity);

    return table;
}

void resize(HashTable **table_ptr) {
    HashTable *table = *table_ptr;
    int threshold = table->capacity * LOAD_FACTOR;
//    printf("%d/%d", table->node_count, threshold);
    if (table->node_count >= threshold) {
//        printf(" (Resize)\n");
        HashTable *new_table = hashtable_create(table->capacity * 2);
        for (int i = 0; i < table->capacity; i++) {
            Node *node = table->nodes[i];
            while (node) {
                hash_table_insert(&new_table, node->key, node->value);
                node = node->next_collision;
            }
        }
        *table_ptr = new_table;
        return;
    }
//    printf(" (No resize)\n");
}

void destroy_table(HashTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        destroy_node(table->nodes[i]);
    }

    free(table->nodes);
    free(table);
}

int insert_collision(Node *node, char *key, char *value) {
    if (strcmp(key, node->key) == 0) {
        free(node->value);
        node->value = calloc( sizeof(char), strlen(value) + 1);
        strcpy(node->value, value);
        return 1;
    }

    if (node->next_collision != NULL) {
        insert_collision(node->next_collision, key, value);
        return 0;
    }
    node->next_collision = node_create(key, value);
    return 0;
}

void hash_table_insert(HashTable **table_ptr, char *key, char *value) {
    resize(table_ptr);
    HashTable *table = *table_ptr;
    table->node_count++;
    unsigned long index = adler32(key, strlen(key));
//    printf("%s = %lu\n", key, index);
    index = index % table->capacity;

    if (table->nodes[index] != NULL) {
        table->node_count -= insert_collision(table->nodes[index], key, value);
        return;
    }

    table->nodes[index] = node_create(key, value);
}

char *find_collision(Node *node, char *key) {
    if (!node) return NULL;
    if (strcmp(node->key, key) != 0) return find_collision(node->next_collision, key);
    if (strcmp(node->key, key) == 0) return node->value;
    return NULL;
}

char *hash_table_find(HashTable *table, char *key) {
    unsigned long index = adler32(key, strlen(key)) % table->capacity;
    Node *node = table->nodes[index];

    if (!node || (!node->next_collision && strcmp(node->key, key) != 0)) {
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

double hash_table_get_fill_factor(HashTable *table) {
    return (double)table->node_count/table->capacity;
}

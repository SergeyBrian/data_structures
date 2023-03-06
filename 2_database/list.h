#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <limits.h>

#define STR(x) #x
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define list_iter(list) for(Node * it = list->begin; it != NULL; it = it->next)
#define cast(type, n) (type) n

struct Node {
    long long value;
    struct Node * next;
};

struct List {
    struct Node * begin;
    int length;
    int sorted;
};

typedef struct Node Node;
typedef struct List List;

List * list_init();
List * list_init_size(int length);
List * list_init_values(int length, long long values[]);
int list_len(List * src);
void list_prepend(List * dest, long long value);
void list_append(List * dest, long long value);
void list_insert(List * dest, int pos, long long value);
Node * list_get_node(List * src, int pos);
long long list_get(List * src, int pos);
void list_replace(List * dest, int pos, long long value);
void list_remove_first(List * dest);
void list_remove_last(List * dest);
void list_remove(List * dest, int pos);
char * list_to_str(List * src);
int list_index(List * src, long long value);
void list_swap(List * dest, int pos1, int pos2);
void list_sort(List * dest, int desc);
int list_compare(List * src_first, List * src_second);
void list_merge(List * dest, List * src);
void list_clear(List * dest);
void list_destroy(List * dest);
int list_count(List * src, long long value);
void list_remove_value(List * dest, long long value, int count);
void list_reverse(List * dest);
long long list_sum(List * src);
long long list_product(List * src);
long long list_min(List * src);
long long list_max(List * src);
List * list_filter(List * src, int (* func)(long long value));
int list_all(List * src, int (* func)(long long value));
int list_any(List * src, int (* func)(long long value));
void list_map(List * dest, long long (* func)(long long value));

#endif

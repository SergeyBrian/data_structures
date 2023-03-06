#include "list.h"
#include "utils.h"

List *list_init() {
    List *list = (List *) mmalloc(sizeof(List));
    list->begin = NULL;
    list->length = 0;
    list->sorted = 1;
    return list;
}

List *list_init_size(int length) {
    List *list = list_init();
    for (int i = 0; i < length; i++) {
        list_append(list, 0);
    }
    return list;
}

List *list_init_values(int length, long long values[]) {
    List *list = list_init();
    for (int i = 0; i < length; i++) {
        list_append(list, values[i]);
    }
    return list;
}

void list_append(List *list, long long value) {
    list->length++;
    if (list->begin == NULL) {
        list->begin = (Node *) mmalloc(sizeof(Node));
        list->begin->value = value;
        list->begin->next = NULL;
        return;
    }

    Node *it = list->begin;
    while (it->next != NULL) {
        it = it->next;
    }
    it->next = (Node *) mmalloc(sizeof(Node));
    it->next->value = value;
    it->next->next = NULL;

    if (!list->sorted) return;
    list->sorted = ((list->sorted == 1 & it->value < it->next->value) ||
                    (list->sorted == -1 & it->value > it->next->value)) ? list->sorted : 0;
}

int list_len(List *list) {
    return list->length;
}


void list_prepend(List *list, long long value) {
    if (list->begin == NULL) {
        list_append(list, value);
        return;
    }

    list->length++;
    Node *first = list->begin;
    Node *new_element = (Node *) mmalloc(sizeof(Node));

    list->begin = new_element;
    new_element->value = value;
    new_element->next = first;

    if (!list->sorted) return;
    list->sorted = ((list->sorted == 1 & list->begin->value < list->begin->next->value) ||
                    (list->sorted == -1 & list->begin->value > list->begin->next->value)) ? list->sorted : 0;
}


Node *list_get_node(List *list, int pos) {
    if (pos < 0) {
        pos = list->length + pos;
    }
    if (list->begin == NULL || pos >= list_len(list)) {
        return NULL;
    }
    int i = 0;
    Node *node = list->begin;
    list_iter(list) {
        node = it;
        if (i == pos) {
            break;
        }
        i++;
    }
    return node;
}

long long list_get(List *list, int pos) {
    return list_get_node(list, pos)->value;
}

void list_replace(List *list, int pos, long long value) {
    list_get_node(list, pos)->value = value;
}

void list_insert(List *list, int pos, long long value) {

    if (pos == 0) {
        list_prepend(list, value);
        return;
    }

    if (pos >= list_len(list)) {
        return;
    }

    if (list->begin == NULL) {
        list_append(list, value);
        return;
    }

    list->length++;

    Node *new_element = (Node *) mmalloc(sizeof(Node));

    Node *prev_element = list_get_node(list, pos - 1);
    Node *next_element = prev_element->next;

    prev_element->next = new_element;
    new_element->next = next_element;
    new_element->value = value;

    if (!list->sorted) return;
    list->sorted = ((list->sorted == 1 & prev_element->value < new_element->value &
                     new_element->value < next_element->value) ||
                    (list->sorted == -1 & prev_element->value > new_element->value &
                     new_element->value > next_element->value)) ? list->sorted : 0;
}

void list_remove_first(List *list) {
    if (list->begin == NULL) {
        return;
    }
    list->length--;
    if (list->begin->next == NULL) {
        list->begin = NULL;
        return;
    }

    Node *next_element = list->begin->next;
    mfree(list->begin);
    list->begin = next_element;
}

void list_remove_last(List *list) {
    if (list->begin == NULL) {
        return;
    }

    if (list_len(list) <= 1) {
        list_remove_first(list);
        return;
    }

    Node *prev = list_get_node(list, list_len(list) - 2);
    mfree(prev->next);
    list->length--;
    prev->next = NULL;
}

void list_remove(List *list, int pos) {
    if (pos == 0) {
        list_remove_first(list);
        return;
    }

    if (pos == list_len(list) - 1) {
        list_remove_last(list);
        return;
    }

    list->length--;

    Node *prev_element = list_get_node(list, pos - 1);
    Node *rm_element = prev_element->next;
    Node *next_element = rm_element->next;

    mfree(rm_element);
    prev_element->next = next_element;
}

char *list_to_str(List *list) {
    char *result = (char *) mmalloc(sizeof(char) * (list_len(list) + 1));
    int len = list_len(list);
    for (int i = 0; i < len; i++) {
        result[i] = (char) list_get(list, i);
    }
    result[len] = '\0';
    return result;
}

int list_index(List *list, long long int value) {
    if (list->begin == NULL) return -1;

    int pos = 0;
    list_iter(list) {
        if (it->value == value) return pos;
        if (list->sorted) {
            switch (list->sorted) {
                case -1:
                    if (value < it->value) return -1;
                    break;
                case 1:
                    if (value > it->value) return 1;
                    break;
            }
        }
        pos++;
    }
    return -1;
}

void list_swap(List *list, int pos1, int pos2) {

    Node *first = list_get_node(list, pos1);
    Node *second = list_get_node(list, pos2);
    long long tmp = second->value;
    second->value = first->value;
    first->value = tmp;
    list->sorted = 0;
}

void merge_sort(List *list, List *buff, int l, int r, int desc) {
    if (l < r) {
        int m = (l + r) / 2;
        merge_sort(list, buff, l, m, desc);
        merge_sort(list, buff, m + 1, r, desc);

        int k = l;
        for (int i = l, j = m + 1; i <= m || j <= r;) {
            if (j > r || (i <= m & ((desc) ? 1 : -1) * list_get(list, i) > ((desc) ? 1 : -1) * list_get(list, j))) {
                list_replace(buff, k, list_get(list, i));
                i++;
            } else {
                list_replace(buff, k, list_get(list, j));
                j++;
            }
            k++;
        }
        for (int i = l; i <= r; i++) {
            list_replace(list, i, list_get(buff, i));
        }
    }
}

void list_sort(List *list, int desc) {
    List *buffer = list_init_size(list_len(list));
    merge_sort(list, buffer, 0, list_len(list) - 1, desc);
    list->sorted = (desc) ? -1 : 1;
}

int list_compare(List *list_first, List *list_second) {
    List *first = list_first;
    List *second = list_second;

    if (first->length != second->length) return 0;

    for (int i = 0; i < first->length - 1; i++) {
        if (list_get(list_first, i) != list_get(list_second, i)) return 0;
    }

    return 1;
}

void list_merge(List *dest, List *src) {
    List *list1 = dest;
    List *list2 = src;
    list_iter(list2) {
        list_append(list1, it->value);
    }

    if (list1->sorted == list2->sorted != 0) {
        switch (list1->sorted) {
            case -1:
                list1->sorted = (list_get(dest, -1) > list_get(src, 0)) ? list1->sorted : 0;
                break;
            case 1:
                list1->sorted = (list_get(dest, -1) < list_get(src, 0)) ? list1->sorted : 0;
                break;
        }
    }
}

void destroy_node(Node *node) {
    if (node->next != NULL) {
        destroy_node(node->next);
    }
    mfree(node);
    node->next = NULL;
}

void list_clear(List *list) {
    destroy_node(list->begin);
    list->begin = NULL;
    list->length = 0;
    list->sorted = 1;
}

void list_destroy(List *list) {
    list_clear(list);
    mfree(list);
}

int list_count(List *list, long long value) {
    int result = 0;
    list_iter(list) {
        if (it->value == value) {
            result++;
        }
    }

    return result;
}

// If count is 0, remove all occurrences, else remove first 'count' elements
void list_remove_value(List *list, long long value, int count) {
    int pos = 0;
    if (count == 0) {
        count = -1;
    }
    list_iter(list) {
        if (count == 0) {
            return;
        }
        if (it->value == value) {
            list_remove(list, pos);
            count--;
            list_remove_value(list, value, count);
            return;
        }
        pos++;
    }
}

void list_reverse(List *list) {
    int pre_sorted_flag = list->sorted;
    for (int i = 0; i < list->length / 2; i++) {
        list_swap(list, i, -1 - i);
    }
    list->sorted = pre_sorted_flag * -1;
}

long long list_sum(List *list) {
    long long result = 0;

    list_iter(list) {
        result += it->value;
    }

    return result;
}

long long list_product(List *list) {
    long long result = 1;

    list_iter(list) {
        result *= it->value;
    }

    return result;
}

long long list_min(List *list) {
    long long result = LLONG_MAX;

    list_iter(list) {
        if (it->value < result) {
            result = it->value;
        }
    }

    return result;
}

long long list_max(List *list) {
    long long result = LLONG_MIN;

    list_iter(list) {
        if (it->value > result) {
            result = it->value;
        }
    }

    return result;
}

List *list_filter(List *list, int (*func)(long long value)) {
    List *new_list = list_init();

    list_iter(list) {
        if (func(it->value)) {
            list_append(new_list, it->value);
        }
    }

    return new_list;
}

int list_all(List *list, int (*func)(long long value)) {
    list_iter(list) {
        if (!func(it->value)) {
            return 0;
        }
    }

    return 1;
}

int list_any(List *list, int (*func)(long long value)) {
    list_iter(list) {
        if (func(it->value)) {
            return 1;
        }
    }

    return 0;
}

void list_map(List *list, long long (*func)(long long value)) {
    list_iter(list) {
        it->value = func(it->value);
    }
    list->sorted = 0;
}

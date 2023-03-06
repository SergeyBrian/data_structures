#include "utils.h"

char error_messages[][100] = {
        "Некорректные параметры",
        "Некорректный формат команды",
        "Некорректный оператор в команде",
        "Некорректное название поля в команде"
};

int malloc_count = 0;
int calloc_count = 0;
int free_count = 0;

void error_exit(exitcode e) {
    printf("%s\n", error_messages[e]);
    exit(e);
}


void *mmalloc(size_t size) {
    malloc_count++;
    return malloc(size);
}

void *mcalloc(size_t type, int n) {
    calloc_count++;
    return calloc(type, n);
}

void mfree(void *ptr) {
    free_count++;
    free(ptr);
}

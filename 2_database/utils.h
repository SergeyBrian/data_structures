#ifndef INC_2_DATABASE_UTILS_H
#define INC_2_DATABASE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDEX_OF(ARR, LEN, ELEMENT) \
    ({ \
        int INDEX = -1; \
        for (int I = 0; I < (LEN); I++) { \
            if ((ARR)[I][0] == (ELEMENT)) { \
                INDEX = I; \
                break; \
            } \
        } \
        INDEX; \
    })

extern char error_messages[][100];

extern int malloc_count;

extern int calloc_count;

extern int free_count;

void *mmalloc(size_t size);

void *mcalloc(size_t type, int n);

void mfree(void *ptr);

typedef enum {
    ERR_INVALID_PARAMS,
    ERR_INVALID_COMMAND,
    ERR_INVALID_OPERATOR,
    ERR_INVALID_FIELD_NAME
} exitcode;

void error_exit(exitcode e);

#endif //INC_2_DATABASE_UTILS_H

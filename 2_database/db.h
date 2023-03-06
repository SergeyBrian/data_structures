#ifndef INC_2_DATABASE_DB_H
#define INC_2_DATABASE_DB_H

#include <string.h>
#include <ctype.h>

#include "list.h"
#include "utils.h"


extern char status_names[][64];

typedef enum {
    normal,
    only_incoming_calls,
    no_calls,
    disabled,
    USER_STATUS_COUNT
} UserStatus;

typedef struct {
    char *last_name;
    char *first_name;
    char *middle_name;
    unsigned long long phone;
    float money;
    float min_money;
    UserStatus status;
} DBRecord;

typedef enum {
    EQUAL,
    LESS,
    GREATER,
    IN,
    INCLUDE,
    IS_EQUAL,
    NOT_EQUAL,
    OPERATORS_COUNT
} Operator;

extern char operator_symbols[OPERATORS_COUNT][10];

typedef struct {
    char *left;
    char *right;
    Operator op;
} Statement;

List *db_init();

void db_destroy(List *DB);

void db_insert(List *DB, char *params);

#endif //INC_2_DATABASE_DB_H

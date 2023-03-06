#include "db.h"


#define MAX_BUFF_SIZE 64

char status_names[][MAX_BUFF_SIZE] = {
        "normal",
        "only_incoming_calls",
        "no_calls",
        "disabled"
};

char field_names[][MAX_BUFF_SIZE] = {
        "last_name",
        "first_name",
        "middle_name",
        "phone",
        "money",
        "min_money",
        "status"
};

char operator_symbols[OPERATORS_COUNT][10] = {
        "=",
        "<",
        ">",
        "/in/",
        "/include/",
        "==",
        "!="
};

typedef enum {
    FIRST,
    SECOND,
    STATEMENT_STATE_COUNT
} StatementState;

typedef enum {
    LAST_NAME,
    FIRST_NAME,
    MIDDLE_NAME,
    PHONE,
    MONEY,
    MIN_MONEY,
    STATUS,
    FIELD_NAME_COUNT
} FieldName;

List *db_init() {
    return list_init();
}

void db_destroy(List *DB) {
    list_destroy(DB);
}

FieldName str_to_field(char *s) {
    for (int i = 0; i < FIELD_NAME_COUNT; i++) {
        if (strcmp(s, field_names[i]) == 0) return (FieldName) i;
    }
}

UserStatus str_to_status(char *s) {
    for (int i = 0; i < USER_STATUS_COUNT; i++) {
        if (strcmp(s, status_names[i]) == 0) return (UserStatus) i;
    }
}

int eval_statement(DBRecord *rec, Statement statement) {
    switch (statement.op) {
        case EQUAL:
            switch (str_to_field(statement.left)) {
                case LAST_NAME:
                    strcpy(rec->last_name, statement.right);
                    break;
                case FIRST_NAME:
                    strcpy(rec->first_name, statement.right);
                    break;
                case MIDDLE_NAME:
                    strcpy(rec->middle_name, statement.right);
                    break;
                case PHONE:
                    rec->phone = atoi(statement.right);
                    break;
                case MONEY:
                    rec->money = atof(statement.right);
                    break;
                case MIN_MONEY:
                    rec->min_money = atof(statement.right);
                    break;
                case STATUS:
                    rec->status = str_to_status(statement.right);
                    break;
                default:
                    error_exit(ERR_INVALID_FIELD_NAME);
            }
            break;
        case LESS:
            break;
        case GREATER:
            break;
        case IN:
            break;
        case INCLUDE:
            break;
        case IS_EQUAL:
            return strcmp(statement.left, statement.right) == 0;
            break;
        case NOT_EQUAL:
            return strcmp(statement.left, statement.right) != 0;
            break;
        default:
            error_exit(ERR_INVALID_OPERATOR);
    }
}

void db_insert(List *DB, char *params) {
    DBRecord *record = malloc(sizeof(DBRecord));

    int l = strlen(params);
    char buff[MAX_BUFF_SIZE] = {};
    int buff_size = 0;
    Statement statement = {NULL, NULL, EQUAL};
    statement.left = mcalloc(sizeof(char), MAX_BUFF_SIZE);
    statement.right = mcalloc(sizeof(char), MAX_BUFF_SIZE);
    StatementState state = FIRST;
    for (int i = 0; i < l && params[i] != '\n'; i++) {
        if (isalnum(params[i]) || params[i] == '_' || params[i] == '.') {
            buff[buff_size++] = params[i];
            continue;
        }
        if (INDEX_OF(operator_symbols, OPERATORS_COUNT, params[i]) == -1) {
            state = (state + 1) % STATEMENT_STATE_COUNT;
            if (statement.left == NULL || statement.right == NULL) error_exit(ERR_INVALID_COMMAND);
            eval_statement(record, statement);
            statement.right = NULL;
            statement.left = NULL;
        }
        if (state == FIRST) {
            strcpy(statement.left, buff);
            state = SECOND;
        }

    }

    free(statement.left);
    free(statement.right);
    list_append(DB, cast(long long, record));
}

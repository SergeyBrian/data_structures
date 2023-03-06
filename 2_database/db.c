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
    list_iter(DB) {
        mfree((DBRecord *) it->value);
    }
    list_destroy(DB);
}

typedef struct {
    char* field_name;
    void* field_ptr;
    char field_type;
} FieldInfo;

void db_insert(List *DB, char *values_str) {
    DBRecord *record = malloc(sizeof(DBRecord));

    FieldInfo field_infos[] = {
            {"last_name", &record->last_name, 's'},
            {"first_name", &record->first_name, 's'},
            {"middle_name", &record->middle_name, 's'},
            {"phone", &record->phone, 'l'},
            {"money", &record->money, 'f'},
            {"min_money", &record->min_money, 'f'},
            {"status", &record->status, 'e'},
    };
    int num_fields = sizeof(field_infos) / sizeof(FieldInfo);

    char* str_copy = strdup(values_str);
    char* field_str = strtok(str_copy, ",");
    while (field_str != NULL) {
        char* equals_sign = strchr(field_str, '=');
        if (equals_sign == NULL) {
            error_exit(ERR_INVALID_FIELD_NAME);
        }
        *equals_sign = '\0';
        char* field_name = field_str;
        char* value_str = equals_sign + 1;

        FieldInfo* field_info = NULL;
        for (int i = 0; i < num_fields; i++) {
            if (strcmp(field_infos[i].field_name, field_name) == 0) {
                field_info = &field_infos[i];
                break;
            }
        }
        if (field_info == NULL) {
            error_exit(ERR_INVALID_FIELD_NAME);
        }

        switch (field_info->field_type) {
            case 's':
                *(char**)field_info->field_ptr = strdup(value_str);
                break;
            case 'l':
                *(unsigned long long*)field_info->field_ptr = (unsigned long long) atoll(value_str);
                break;
            case 'f':
                *(float*)field_info->field_ptr = atof(value_str);
                break;
            case 'e':
                if (strcmp(value_str, "normal") == 0) {
                    *(UserStatus*)field_info->field_ptr = normal;
                } else if (strcmp(value_str, "only_incoming_calls") == 0) {
                    *(UserStatus*)field_info->field_ptr = only_incoming_calls;
                } else if (strcmp(value_str, "no_calls") == 0) {
                    *(UserStatus*)field_info->field_ptr = no_calls;
                } else if (strcmp(value_str, "disabled") == 0) {
                    *(UserStatus*)field_info->field_ptr = disabled;
                } else {
                    error_exit(ERR_INVALID_ENUM_VALUE);
                }
                break;
            default:
                error_exit(ERR_INVALID_FIELD_NAME);
                break;
        }

        field_str = strtok(NULL, ",");
    }

    free(str_copy);

    list_append(DB, cast(long long, record));
}

#include "db.h"


#define MAX_BUFF_SIZE 64

#define MAX_RECORDS 100000

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
        DBRecord *rec = cast(DBRecord *, it->value);
        mfree(rec->first_name);
        mfree(rec->last_name);
        mfree(rec->middle_name);
        mfree(rec);
    }
    list_destroy(DB);
}

typedef struct {
    char *field_name;
    void *field_ptr;
    char field_type;
} FieldInfo;

void db_insert(List *DB, char *values_str) {
    DBRecord *record = mmalloc(sizeof(DBRecord));

    FieldInfo field_infos[] = {
            {"last_name",   &record->last_name,   's'},
            {"first_name",  &record->first_name,  's'},
            {"middle_name", &record->middle_name, 's'},
            {"phone",       &record->phone,       'l'},
            {"money",       &record->money,       'f'},
            {"min_money",   &record->min_money,   'f'},
            {"status",      &record->status,      'e'},
    };
    int num_fields = sizeof(field_infos) / sizeof(FieldInfo);

    char *str_copy = mstrdup(values_str);
    char *field_str = strtok(str_copy, ",");
    while (field_str != NULL) {
        char *equals_sign = strchr(field_str, '=');
        if (equals_sign == NULL) {
            error_exit(ERR_INVALID_FIELD_NAME);
        }
        *equals_sign = '\0';
        char *field_name = field_str;
        char *value_str = equals_sign + 1;

        FieldInfo *field_info = NULL;
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
                *(char **) field_info->field_ptr = mstrdup(value_str);
                break;
            case 'l':
                *(unsigned long long *) field_info->field_ptr = (unsigned long long) atoll(value_str);
                break;
            case 'f':
                *(float *) field_info->field_ptr = atof(value_str);
                break;
            case 'e':
                if (strcmp(value_str, "normal") == 0) {
                    *(UserStatus *) field_info->field_ptr = normal;
                } else if (strcmp(value_str, "only_incoming_calls") == 0) {
                    *(UserStatus *) field_info->field_ptr = only_incoming_calls;
                } else if (strcmp(value_str, "no_calls") == 0) {
                    *(UserStatus *) field_info->field_ptr = no_calls;
                } else if (strcmp(value_str, "disabled") == 0) {
                    *(UserStatus *) field_info->field_ptr = disabled;
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

    mfree(str_copy);

    list_append(DB, cast(long long, record));
}

int check_equal_condition(DBRecord *rec, char *condition) {
    char *field_name;
    char *value;
    char *token;

    int result = 0;

    token = strtok(condition, "==");
    if (token != NULL) {
        field_name = token;
        token = strtok(NULL, "'");
        if (strcmp(token, "=") == 0) token = strtok(NULL, "'");
        else {
            token++;
        }
        if (token != NULL) {
            value = token;

            if (strcmp(field_name, "last_name") == 0) {
                if (strcmp(rec->last_name, value) == 0) {
                    result = 1;
                }
            } else if (strcmp(field_name, "first_name") == 0) {
                if (strcmp(rec->first_name, value) == 0) {
                    result = 1;
                }
            } else if (strcmp(field_name, "middle_name") == 0) {
                if (strcmp(rec->middle_name, value) == 0) {
                    result = 1;
                }
            } else if (strcmp(field_name, "phone") == 0) {
                if (rec->phone == strtoull(value, NULL, 10)) {
                    result = 1;
                }
            } else if (strcmp(field_name, "money") == 0) {
                if (rec->money == strtof(value, NULL)) {
                    result = 1;
                }
            } else if (strcmp(field_name, "min_money") == 0) {
                if (rec->min_money == strtof(value, NULL)) {
                    result = 1;
                }
            } else if (strcmp(field_name, "status") == 0) {
                if (strcmp(value, "normal") == 0 && rec->status == normal) {
                    result = 1;
                } else if (strcmp(value, "only_incoming_calls") == 0 && rec->status == only_incoming_calls) {
                    result = 1;
                } else if (strcmp(value, "no_calls") == 0 && rec->status == no_calls) {
                    result = 1;
                } else if (strcmp(value, "disabled") == 0 && rec->status == disabled) {
                    result = 1;
                }
            }
        }
    }

    return result;
}

int check_compare_condition(DBRecord *rec, char *condition, char *type) {
    char *field_name;
    char *value;
    char *token;

    int result = 0;


    token = strtok(condition, type);
    if (token != NULL) {
        field_name = token;
        token = strtok(NULL, "'");
        if (token != NULL) {
            value = token;

            if (strcmp(field_name, "money") == 0) {
                if (COMPARE(rec->money, strtof(value, NULL), type[0])) {
                    result = 1;
                }
            } else if (strcmp(field_name, "min_money") == 0) {
                if (COMPARE(rec->min_money, strtof(value, NULL), type[0])) {
                    result = 1;
                }
            }
        }
    }

    return result;
}

int check_in_condition(DBRecord *rec, char *condition) {
    int result = 0;

    char *token = strtok(condition, "/in/");
    char *field_name;
    if (token != NULL) {
        field_name = token;
        if (strcmp(field_name, "status")) error_exit(ERR_INVALID_COMMAND);
        char *value_list = field_name + strlen(field_name) + 4;
        char *value = strtok(value_list, "', ");
        while (value) {
            if (strcmp(value, status_names[rec->status]) == 0) result = 1;
            value = strtok(NULL, "', ");
        }

        int x = 0;
    }


    return result;
}

int check_condition(DBRecord *rec, char *condition) {
    int result = 0;
    int compare_type = 0;

    if (strstr(condition, "/in/") != NULL) return check_in_condition(rec, condition);
    if (strstr(condition, "==") != NULL) return check_equal_condition(rec, condition);

    if (strstr(condition, ">") != NULL) compare_type = 1;
    if (strstr(condition, "<") != NULL) compare_type = 2;

    if (compare_type) result = check_compare_condition(rec, condition, (compare_type == 1) ? ">" : "<");

    return result;

}

int validate_rec(DBRecord *rec, char *conditions) {
    //conditions == "last_name=='Ivan',money>100,status/in/['normal', 'only_incoming_calls']"
    char buf[MAX_INPUT];
    int l = 0;
    while (l < strlen(conditions)) {
        sscanf(conditions + l, "%[^,[]", buf);
        l += strlen(buf);

        if (conditions[l++] == '[') {
            int buf_len = strlen(buf);
            sscanf(conditions + l, "%[^]]", buf + strlen(buf));
            l += strlen(buf) - buf_len + 2;
            if (!check_condition(rec, buf)) return 0;
            continue;
        }
        if (!check_condition(rec, buf)) return 0;
    }

    return 1;
}

List *db_select(List *BD, int n, char *conditions) {
    int count = 0;
    List *result = list_init();

    list_iter(BD) {
        if (n && count == n) {
            break;
        }

        if (!validate_rec(cast(DBRecord *, it->value), conditions)) continue;
        count++;
        list_append(result, cast(long long, it->value));
    }

    return result;
}

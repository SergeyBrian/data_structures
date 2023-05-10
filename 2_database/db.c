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

void record_destroy(DBRecord *rec) {
    mfree(rec->first_name);
    mfree(rec->last_name);
    mfree(rec->middle_name);
    mfree(rec);
}

void db_destroy(List *DB) {
    list_iter(DB) {
        DBRecord *rec = cast(DBRecord *, it->value);
        record_destroy(rec);
    }
    list_destroy(DB);
}

typedef struct {
    char *field_name;
    void *field_ptr;
    char field_type;
} FieldInfo;

void record_set_values(DBRecord *record, char *values_str) {
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
}

void db_insert(List *DB, char *values_str) {
    DBRecord *record = mmalloc(sizeof(DBRecord));

    record_set_values(record, values_str);


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

int db_remove(List *DB, int n, char *conditions) {
    int count = 0;

    list_iter(DB) {
        if (n && count == n) {
            break;
        }

        if (!validate_rec(cast(DBRecord *, it->value), conditions)) continue;
        record_destroy(cast(DBRecord *, it->value));
        list_remove_value(DB, it->value, 1);
        count++;
        if (!list_len(DB)) break;
    }

    return count;
}

int db_update(List *DB, char *values, char *conditions) {
    int count = 0;

    list_iter(DB) {
        DBRecord *rec = cast(DBRecord *, it->value);
        if (!validate_rec(rec, conditions)) continue;
        record_set_values(rec, values);
        count++;
    }

    return count;
}

int db_uniq(List *DB, char *fields) {
    int count = 0;

    int remove_list_size = 0;
    char *remove_list[1000];
    int remove_counts[1000];
    for (int i = 0; i < 1000; i++) {
        remove_list[i] = calloc(sizeof(char), MAX_INPUT);
    }

    list_iter(DB) {
        char *tmp_fields = mstrdup(fields);
        char *save;
        char *field = strtok_r(tmp_fields, ",", &save);
        DBRecord *rec = cast(DBRecord *, it->value);
        char condition[MAX_INPUT] = "";
        List *matches = NULL;
        while (field) {
            sprintf(condition+strlen(condition), "%s==", field);
            if (strcmp(field, "first_name") == 0) {
                sprintf(condition + strlen(condition), "%s,", rec->first_name);
            }
            else if (strcmp(field, "middle_name") == 0) {
                sprintf(condition + strlen(condition), "%s,", rec->middle_name);
            }
            else if (strcmp(field, "last_name") == 0) {
                sprintf(condition + strlen(condition), "%s,", rec->last_name);
            }
            else if (strcmp(field, "phone") == 0) {
                sprintf(condition + strlen(condition), "%llu,", rec->phone);
            }
            else if (strcmp(field, "money") == 0) {
                sprintf(condition + strlen(condition), "%g,", rec->money);
            }
            else if (strcmp(field, "min_money") == 0) {
                sprintf(condition + strlen(condition), "%g,", rec->min_money);
            }
            else if (strcmp(field, "status") == 0) {
                sprintf(condition + strlen(condition), "%s,", status_names[rec->status]);
            }
            field = strtok_r(NULL, ",", &save);
        }
        condition[strlen(condition)] = 0;
        matches = db_select(DB, 0, condition);

        int match_count = list_len(matches);
        if (match_count > 1) {
            strcpy(remove_list[remove_list_size], condition);
            remove_counts[remove_list_size++] = match_count - 1;
        }
        list_destroy(matches);
        mfree(tmp_fields);
    }

    for (int i = 0; i < remove_list_size; i++) {
        count += db_remove(DB, remove_counts[i], remove_list[i]);
    }

    return count;
}

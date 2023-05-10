#include <stdio.h>

#include "list.h"
#include "db.h"
#include "utils.h"

#define MAX_PARAM_SIZE 40
#define MAX_PARAMS_COUNT 4

#define PARAM_INPUT_FILE_NAME 0


void parse_params(char **params, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    if (i + 1 >= argc) error_exit(ERR_INVALID_PARAMS);
                    params[PARAM_INPUT_FILE_NAME] = argv[i + 1];
            }
        }
    }
}

int main(int argc, char **argv) {
    char *params[MAX_PARAMS_COUNT];
    parse_params(params, argc, argv);

    List *DB = db_init();

    FILE *input_file = fopen(params[PARAM_INPUT_FILE_NAME], "r");

    char command[MAX_INPUT] = "";
    while (fgets(command, MAX_INPUT, input_file)) {
        char *args = strchr(command, ' ');
        args[0] = 0;
        args++;
        if (args[strlen(args) - 1] == '\n') {
            args[strlen(args) - 1] = 0;
        }

        if (strcmp(command, "insert") == 0) {
            db_insert(DB, args);
        } else if (strcmp(command, "select") == 0) {
            List *recs = db_select(DB, 0, args);
            printf("Select: %d\n", list_len(recs));
            list_iter(recs) {
                DBRecord *rec = cast(DBRecord *, it->value);
                printf("first_name=%s middle_name=%s last_name=%s phone=%llu money=%g min_money=%g status=%s\n",
                       rec->first_name, rec->middle_name, rec->last_name, rec->phone, rec->money, rec->min_money,
                       status_names[rec->status]);
            }
            list_destroy(recs);
        } else if (strcmp(command, "delete") == 0) {
            printf("Delete: %d\n", db_remove(DB, 0, args));
        } else if (strcmp(command, "update") == 0) {
            char *args2 = strchr(args, ' ');
            args2[0] = 0;
            args2++;
            printf("Update: %d\n", db_update(DB, args, args2));
        } else if (strcmp(command, "uniq") == 0) {
            printf("Uniq: %d\n", db_uniq(DB, args));
        }
    }

    db_destroy(DB);


    printf("\n======================\n");
    printf("malloc: %d\ncalloc: %d\nfree: %d\nmalloc + calloc - free: %d", malloc_count, calloc_count, free_count,
           malloc_count + calloc_count - free_count);

    return 0;
}

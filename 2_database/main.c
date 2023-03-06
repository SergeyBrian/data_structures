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
                    params[PARAM_INPUT_FILE_NAME] = argv[i+1];
            }
        }
    }
}

int main(int argc, char **argv) {
    char params[MAX_PARAM_SIZE][MAX_PARAMS_COUNT];
    parse_params(params, argc, argv);

    List *DB = db_init();


    db_insert(DB, "first_name=hello,last_name=fucker,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");


    db_destroy(DB);
    printf("%d\n%d\n%d\n", malloc_count, calloc_count, free_count);

    return 0;
}

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
        printf("%s", command);
    }

    db_destroy(DB);


    printf("\n======================\n");
    printf("malloc: %d\ncalloc: %d\nfree: %d\nmalloc + calloc - free: %d", malloc_count, calloc_count, free_count,
           malloc_count + calloc_count - free_count);

    return 0;
}

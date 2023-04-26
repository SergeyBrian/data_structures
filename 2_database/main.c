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
    char params[MAX_PARAM_SIZE][MAX_PARAMS_COUNT];
    parse_params(params, argc, argv);

    List *DB = db_init();


    db_insert(DB,
              "first_name=hello,last_name=Ivan,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");
    db_insert(DB,
              "first_name=hello,last_name=fucker,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");
    db_insert(DB,
              "first_name=hello1,last_name=fucker,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");
    db_insert(DB,
              "first_name=hello2,last_name=fucker,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");
    db_insert(DB,
              "first_name=hello3,last_name=fucker,phone=79998887766,middle_name=ok,money=4.4,min_money=18,status=normal");

    List *recs;

//    recs = db_select(DB, 3, "last_name=='Ivan',money>100,status/in/['normal', 'only_incoming_calls'],min_money<50");
//    recs = db_select(DB, 1, "last_name=='Ivan',money==4.4,phone==79998887766");
    recs = db_select(DB, 0, "money==4.4,middle_name=='ok'");

    list_iter(recs) {
        DBRecord *rec = cast(DBRecord *, it->value);
        printf("%s\n", rec->first_name);
    }

    list_destroy(recs);
    db_destroy(DB);
    printf("\n======================\n");
    printf("malloc: %d\ncalloc: %d\nfree: %d\nmalloc + calloc - free: %d", malloc_count, calloc_count, free_count,
           malloc_count + calloc_count - free_count);

    return 0;
}

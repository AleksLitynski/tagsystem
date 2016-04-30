

#include <stdio.h>
#include "lmdb.h"

typedef struct {
    MDB_env * env;
} mdb_box;

int popit(mdb_box * box) {
    mdb_env_create(&box->env);
    char * hey = malloc(10);
}

int main(int argc, char * argv[]) {
    mdb_box box;
    popit(&box);
    printf("hey\n");

    return 0;
}




#include <stdio.h>
#include "lmdb.h"

int main(int argc, char * argv[]){
    printf("argc: %i\n", argc);
    for(int i = 0; i <= argc; i++){
        printf("hey %s\n", argv[i]);
    }
    

    MDB_env * env;
    mdb_env_create(&env);
    mdb_env_close(env);

    return 0;

}

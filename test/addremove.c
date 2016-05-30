#include <stdio.h>
#include "lmdb.h"
#include <sys/stat.h>

MDB_env * env(char * name) {
    // ts_env_create(path, &env);
    struct stat st = {0};
    if(stat(name, &st) == -1) mkdir(name, 0700);
    MDB_env * menv;
    mdb_env_create(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);
    mdb_env_set_maxdbs(menv, 10000); 

    mdb_env_open(menv, name, 0, 0664);
    return menv;
}    

void doc() {
    // ts_doc_create(&env, id);
}

void tag() {

}

int main(int argc, char * argv[]) {

    printf("Up and running\n");
    char * env_name = "/addremove";

    MDB_env * menv = env(env_name);


    mdb_env_close(menv);
    // ts_tag_insert(&env, id_str, id);
}

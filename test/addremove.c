#include <stdio.h>
#include "lmdb.h"
#include <sys/stat.h>

MDB_env * env_create(char * name) {
    // ts_env_create(path, &env);
    struct stat st = {0};
    if(stat(name, &st) == -1) mkdir(name, 0700);
    MDB_env * env;
    mdb_env_create(&env);
    mdb_env_set_maxreaders(env, 1);
    mdb_env_set_mapsize(env, 10485760);
    mdb_env_set_maxdbs(env, 10000); 

    mdb_env_open(env, name, 0, 0664);
    return env;
}    

void tag_put(MDB_env * env, char * tag, char * tag_data) {
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_CREATE | MDB_INTEGERKEY, &dbi);

    unsigned int zero = 0;
    MDB_val key = {.mv_size = sizeof(unsigned int), .mv_data = &zero};

    MDB_val data = {.mv_size = strlen(tag_data), .mv_data = tag_data};
    int res = mdb_put(txn, dbi, &key, &data, 0);
    mdb_txn_commit(txn);
}
char * tag_get(MDB_env * env, char * tag) {
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_CREATE | MDB_INTEGERKEY, &dbi);

    unsigned int zero = 0;
    MDB_val key = {.mv_size = sizeof(unsigned int), .mv_data = &zero};

    MDB_val data; 
    int res = mdb_get(txn, dbi, &key, &data);
    mdb_txn_commit(txn);

    char * out = malloc(data.mv_size);
    out = strdup(data.mv_data);
    return out;
}

int main(int argc, char * argv[]) {

    printf("Up and running\n");
    char * env_name = "/addremove";

    MDB_env * env = env_create(env_name);

    tag_put(env, "tag_a", "data_a");
    tag_put(env, "tag_a", "data_b");
    tag_put(env, "tag_b", "data_bb");
    tag_put(env, "tag_b", "data_aa");

    printf("tag_a: %s\n", tag_get(env, "tag_a"));
    printf("tag_b: %s\n", tag_get(env, "tag_b"));

    mdb_env_close(env);

}


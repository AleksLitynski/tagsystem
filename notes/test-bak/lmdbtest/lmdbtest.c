#include <stdio.h>
#include "lmdb.h"

int main(int argc, char * argv[]) {
    // create environment
    MDB_env * env;
    mdb_env_create(&env);
    mdb_env_open(env, "./lmdbtestdir", 0, 0664);
    printf("env opened\n");
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_open(txn, NULL, MDB_INTEGERKEY, &dbi);
    printf("trans/db created\n");


    int snum = 123;
    char * sval = "abc";
    MDB_val key  = {.mv_size = sizeof(int), .mv_data = &snum}; 
    MDB_val data = {.mv_size = strlen(sval), .mv_data = sval};
    mdb_put(txn, dbi, &key, &data, 0);
    printf("data inserted\n");

    mdb_txn_commit(txn);
    mdb_env_close(env);
    printf("txn/env closed\n");
    return 0;
}


/*
 char sval[32];

rc = mdb_env_create(&env);
rc = mdb_env_open(env, "./testdb", 0, 0664);
rc = mdb_txn_begin(env, NULL, 0, &txn);
rc = mdb_open(txn, NULL, 0, &dbi);

key.mv_size = sizeof(int);
key.mv_data = sval;
data.mv_size = sizeof(sval);
data.mv_data = sval;

sprintf(sval, "%03x %d foo bar", 32, 3141592);
rc = mdb_put(txn, dbi, &key, &data, 0);
rc = mdb_txn_commit(txn);
*/

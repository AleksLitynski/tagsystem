#include <stdio.h>
#include "lmdb.h"

int main(int argc, char * argv[]) {

    // create environment
    MDB_env * env;
    mdb_env_create(&env);
    mdb_env_set_maxreaders(env, 1);
    mdb_env_set_mapsize(env, 10485760);
    printf("env created\n");

    // open environment
    mdb_env_open(env, "lmdbtestdir", 0, 0664);
    printf("env opened\n");

    // open transaction/create database
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_dbi_open(txn, NULL, MDB_CREATE, &dbi);
    printf("trans/db created\n");

    MDB_val key = {.mv_size = sizeof(unsigned int), .mv_data = (unsigned int)0}; 
    char * data_val = "abcd";
    MDB_val data = {.mv_size = sizeof(data_val), .mv_data = &data_val};
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

#pragma once

// includes
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include "../lib/sds/sds.h"

// macros

// types
typedef struct { 
    sds dir;
    sds docs;
    MDB_env * index;
} ts_db;

// functions
int ts_db_open(ts_db * self, char * path);
int ts_db_close(ts_db * self);

int ts_db_test(ts_db * self, sds db_name, sds key_name);

// the user should commit the transaction when they're done with the value
int ts_db_get(ts_db * self, sds db_name, sds key_name, MDB_val * val, MDB_txn * txn);
int ts_db_put(ts_db * self, sds db_name, sds key_name, MDB_val * val);
#pragma once

// includes
#include "lmdb.h"
#include "sds.h"

#include <ftw.h>

// macros
#define TS_VERSION "0.0.1"

// types
typedef struct {
    sds dir;
    sds docs;
    sds index_path;
    MDB_env * index;
} ts_db;

// functions
int ts_db_open(ts_db * self, char * path);
int ts_db_close(ts_db * self);
int ts_db_DESTROY(ts_db * self);

int ts_db_test(ts_db * self, sds dBb_name, sds key_name);
int ts_db_del(ts_db * self, sds db_name, sds key_name);

// the user should commit the transaction when they're done with the value
int ts_db_get(ts_db * self, sds db_name, sds key_name, MDB_val * val, MDB_txn ** txn);
int ts_db_put(ts_db * self, sds db_name, sds key_name, MDB_val * val);

int _ts_db_delete_fs_item(const char * fpath, const struct stat * sb, int tflag, struct FTW * ftwbuf);
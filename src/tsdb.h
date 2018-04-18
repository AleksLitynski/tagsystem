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

typedef struct {
    char * name;
    unsigned int flags;
} ts_db_table;

typedef struct {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key;
    MDB_cursor * cursor;
} ts_db_iter;

// constantss
const ts_db_table ts_db_index;
const ts_db_table ts_db_iindex;
const ts_db_table ts_db_meta;

// functions
int ts_db_open(ts_db * self, char * path);
int ts_db_close(ts_db * self);
int ts_db_DESTROY(ts_db * self);

int ts_db_test(ts_db * self, ts_db_table * table, char * key_name);
int ts_db_del(ts_db * self, ts_db_table * table, char * key_name, char * value);

// the user should commit the transaction when they're done with the value
int ts_db_get(ts_db * self, ts_db_table * table, char * key_name, MDB_val * val, MDB_txn ** txn);
int ts_db_put(ts_db * self, ts_db_table * table, char * key_name, MDB_val * val);

int _ts_db_delete_fs_item(const char * fpath, const struct stat * sb, int tflag, struct FTW * ftwbuf);

int ts_db_iter_open(ts_db_iter * self, ts_db *  db, ts_db_table * table, char * name);
int ts_db_iter_next(ts_db_iter * self, MDB_val * next);
int ts_db_iter_close(ts_db_iter * self);
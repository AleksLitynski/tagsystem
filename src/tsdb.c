#include "tsdb.h"
#include "tserror.h"
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include "../lib/sds/sds.h"
#include "../lib/fs.c/fs.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

static void ts_db_mkdir(sds path) {
    struct stat st = {0};
    if(stat(path, &st) == -1) {
        fs_mkdir(path, 0700);
    }
}

int ts_db_open(ts_db * self, char * path) {
    self->dir = sdsnew(path);
    self->docs = sdscat(self->dir, "/docs");
    sds index_path = sdscat(self->dir, "/index");

    ts_db_mkdir(self->dir);
    ts_db_mkdir(index_path);
    ts_db_mkdir(self->docs);

    mdb_env_create(&self->index);
    mdb_env_set_maxreaders(self->index, 1);
    mdb_env_set_mapsize(self->index, 10485760);
    mdb_env_set_maxdbs(self->index, 100); // index, inverted index, metadata.

    int res = mdb_env_open(self->index, index_path, 0, 0664);
    printf("mdb_env_open: %s\n", mdb_strerror(res));

    sdsfree(index_path);

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}

int ts_db_close(ts_db * self) {
    sdsfree(self->dir);
    sdsfree(self->docs);
    mdb_env_close(self->index);

    return TS_SUCCESS;
}

int ts_db_test(ts_db * self, sds db_name, sds key_name) {

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, * val;

    key->mv_size = sdslen(key_name);
    key->mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, dbi);

    int res = mdb_get(txn, *dbi, key, val);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_SUCCESS;
}


int ts_db_del(ts_db * self, sds db_name, sds key_name) {

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, * val;

    key->mv_size = sdslen(key_name);
    key->mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, dbi);

    int res = mdb_del(txn, *dbi, key, val);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return TS_SUCCESS;
}

int ts_db_get(ts_db * self, sds db_name, sds key_name, MDB_val * val, MDB_txn * txn) {
    MDB_dbi dbi;
    MDB_val * key;

    key->mv_size = sdslen(key_name);
    key->mv_data = key_name;

    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_get(txn, dbi, key, val);

    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_SUCCESS;
}

int ts_db_put(ts_db * self, sds db_name, sds key_name, MDB_val * val) {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val * key;

    key->mv_size = sdslen(key_name);
    key->mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_put(txn, dbi, key, val, 0);
    mdb_txn_commit(txn);

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}


sds hello_world() {
  return "hello world I'm special";
}

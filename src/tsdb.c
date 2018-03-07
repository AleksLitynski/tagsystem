#include "tsdb.h"
#include "tserror.h"
#include "lmdb.h"
#include "sds.h"
#include "fs.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int ts_db_open(ts_db * self, char * path) {
    srand(time(0));
    self->dir = sdsnew(path);

    self->docs = sdsdup(self->dir);
    self->docs = sdscat(self->docs, "/docs");

    self->index_path = sdsdup(self->dir);
    self->index_path = sdscat(self->index_path, "/index");

    fs_mkdir(self->dir, 0700);
    fs_mkdir(self->index_path, 0700);
    fs_mkdir(self->docs, 0700);

    mdb_env_create(&self->index);
    mdb_env_set_maxreaders(self->index, 1);
    mdb_env_set_mapsize(self->index, 10485760);
    mdb_env_set_maxdbs(self->index, 100); // index, inverted index, metadata.

    int res = mdb_env_open(self->index, self->index_path, 0, 0664);
    LOG("mdb_env_open: %s", mdb_strerror(res));


    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}

int ts_db_close(ts_db * self) {
    sdsfree(self->dir);
    sdsfree(self->docs);
    sdsfree(self->index_path);
    mdb_env_close(self->index);

    return TS_SUCCESS;
}

int ts_db_DESTROY(ts_db * self) {
    sds data = sdsdup(self->index_path);
    data = sdscat(data, "/data.mdb");
    sds lock = sdsdup(self->index_path);
    lock = sdscat(lock, "/lock.mdb");

    unlink(data);
    unlink(lock);
    fs_rmdir(self->index_path);
    fs_rmdir(self->docs);
    fs_rmdir(self->dir);

    sdsfree(data);
    sdsfree(lock);

    ts_db_close(self);
    return TS_SUCCESS;
}

int ts_db_test(ts_db * self, sds db_name, sds key_name) {

    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key, val;

    key.mv_size = sdslen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_get(txn, dbi, &key, &val);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_FAILURE;
}


int ts_db_del(ts_db * self, sds db_name, sds key_name) {

    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key, val;

    key.mv_size = sdslen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_del(txn, dbi, &key, &val);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return TS_SUCCESS;
}

int ts_db_get(ts_db * self, sds db_name, sds key_name, MDB_val * val, MDB_txn * txn) {
    MDB_dbi dbi;
    MDB_val key;

    key.mv_size = sdslen(key_name);
    key.mv_data = key_name;

    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_get(txn, dbi, &key, val);

    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_SUCCESS;
}

int ts_db_put(ts_db * self, sds db_name, sds key_name, MDB_val * val) {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key;

    key.mv_size = sdslen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_CREATE, &dbi);

    int res = mdb_put(txn, dbi, &key, val, 0);
    mdb_txn_commit(txn);

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}
 
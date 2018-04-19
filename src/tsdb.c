#include "tsdb.h"
#include "tserror.h"
#include "lmdb.h"
#include "sds.h"
#include "fs.h"


#include <ftw.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>



const ts_db_table ts_db_index = {.name = "index", .flags = MDB_CREATE | MDB_DUPSORT };
const ts_db_table ts_db_iindex = {.name = "iindex", .flags = MDB_CREATE };
const ts_db_table ts_db_meta = {.name = "meta", .flags = MDB_CREATE };

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

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}

int ts_db_close(ts_db * self) {
    sdsfree(self->dir);
    sdsfree(self->docs);
    sdsfree(self->index_path);
    mdb_env_close(self->index);

    return TS_SUCCESS;
}

int _ts_db_delete_fs_item(const char * fpath, const struct stat * sb, int tflag, struct FTW * ftwbuf) {

    if(S_ISDIR(sb->st_mode)) {
        fs_rmdir(fpath);
    }

    if(!S_ISDIR(sb->st_mode)) {
        unlink(fpath);
    }

    return 0;
}

int ts_db_DESTROY(ts_db * self) {

    nftw(self->dir, _ts_db_delete_fs_item, 200, FTW_DEPTH);

    ts_db_close(self);
    return TS_SUCCESS;
}

int ts_db_test(ts_db * self, ts_db_table * table, char * key_name) {

    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key, val;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, table->name, table->flags, &dbi);

    int res = mdb_get(txn, dbi, &key, &val);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_FAILURE;
}


int ts_db_del(ts_db * self, ts_db_table * table, char * key_name, char * value) {

    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key, val;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, table->name, table->flags, &dbi);

    MDB_val to_del;
    if(value != NULL) {
        to_del.mv_size = strlen(value);
        to_del.mv_data = value;
    }
    int res = mdb_del(txn, dbi, &key, value == NULL ? NULL : &to_del);
    mdb_txn_commit(txn);

    // return 3 for no value found
    return TS_SUCCESS;
}

int ts_db_get(ts_db * self, ts_db_table * table, char * key_name, MDB_val * val, MDB_txn ** txn) {
    MDB_dbi dbi;
    MDB_val key;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    mdb_txn_begin(self->index, NULL, 0, txn);
    mdb_dbi_open(*txn, table->name, table->flags, &dbi);

    int res = mdb_get(*txn, dbi, &key, val);

    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_SUCCESS;
}

int ts_db_put(ts_db * self, ts_db_table * table, char * key_name, MDB_val * val) {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index
    mdb_txn_begin(self->index, NULL, 0, &txn);
    mdb_dbi_open(txn, table->name, table->flags, &dbi);

    int res = mdb_put(txn, dbi, &key, val, 0);
    mdb_txn_commit(txn);

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}
 

int ts_db_iter_open(ts_db_iter * self, ts_db *  db, ts_db_table * table, char * name) {

    self->key.mv_size = strlen(name);
    self->key.mv_data = name;
    mdb_txn_begin(db->index, NULL, 0, &self->txn);
    mdb_dbi_open(self->txn, table->name, table->flags, &self->dbi);
    mdb_cursor_open(self->txn, self->dbi, &self->cursor);

    MDB_val empty;
    mdb_cursor_get(self->cursor, &self->key, &empty, MDB_FIRST);

  return TS_SUCCESS;
}

int ts_db_iter_next(ts_db_iter * self, MDB_val * next) {
    mdb_cursor_get(self->cursor, &self->key, next, MDB_GET_CURRENT);

    MDB_val empty;
    return mdb_cursor_get(self->cursor, &self->key, &empty, MDB_NEXT);
}

int ts_db_iter_close(ts_db_iter * self) {
    mdb_cursor_close(self->cursor);
    mdb_txn_commit(self->txn);
    return TS_SUCCESS;
}
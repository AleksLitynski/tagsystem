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


int ts_db_open(ts_db * self, char * path) {

    // documents have random names, so seed random
    srand(time(0));

    // create db, docs, and index (lmdb database) folders
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
    // I only expect 3 dbs, but it doesn't hurt to have a practically large
    // but computationally low number of dbs available.
    // Expected dbs are index, inverted index, and metadata.
    mdb_env_set_maxdbs(self->index, 100); 

    int res = mdb_env_open(self->index, self->index_path, 0, 0664);

    self->current_txn = 0;

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}

int ts_db_close(ts_db * self) {
    sdsfree(self->dir);
    sdsfree(self->docs);
    sdsfree(self->index_path);
    mdb_env_close(self->index);

    return TS_SUCCESS;
}

int ts_db_begin_txn(ts_db * self) {

    ts_db_txn * txn = calloc(sizeof(ts_db_txn), 1);

    int success = 0;
    if(self->current_txn == 0) {
        success = mdb_txn_begin(self->index, NULL, 0, &txn->txn);
        self->current_txn = txn;
    } else {
        // if we're a nested transaction,
        // push a new txn onto the linked list of txns
        success = mdb_txn_begin(self->index, self->current_txn->txn, 0, &txn->txn);
        txn->parent = self->current_txn;
        self->current_txn = txn;
    }

    // automatically open all three dbs when txn is started. Possibly inefficient, but very convenient
    mdb_dbi_open(self->current_txn->txn, "index", MDB_CREATE | MDB_DUPSORT, &self->current_txn->index_table);
    mdb_dbi_open(self->current_txn->txn, "iindex", MDB_CREATE, &self->current_txn->iindex_table);
    mdb_dbi_open(self->current_txn->txn, "meta", MDB_CREATE, &self->current_txn->meta_table);

    return success;
}

int ts_db_commit_txn(ts_db * self) {

    // close the current transaction and 
    // set the parent transaction to current if it exists
    int success = 0;
    ts_db_txn * current = self->current_txn;
    if(self->current_txn != 0) {
        success = mdb_txn_commit(self->current_txn->txn);
        if(self->current_txn->parent != 0) {
            self->current_txn = self->current_txn->parent;
        } else {
            self->current_txn = 0;
        }
    }
    free(current);

    return success;
}

int _ts_db_delete_fs_item(const char * fpath, const struct stat * sb, int tflag, struct FTW * ftwbuf) {

    // called in ts_db_DESTORY to recursivly delete the database directory
    if(S_ISDIR(sb->st_mode)) {
        fs_rmdir(fpath);
    }

    if(!S_ISDIR(sb->st_mode)) {
        unlink(fpath);
    }

    return 0;
}

int _get_dbi(ts_db * self, char * table) {
    // dispatch the correct MDB_dbi based on a given string.
    // Should replace with an enum to avoid strcmp eventually
    if(strcmp(table, "index")) return self->current_txn->index_table;
    if(strcmp(table, "iindex")) return self->current_txn->iindex_table;
    if(strcmp(table, "meta")) return self->current_txn->meta_table;
}

int ts_db_DESTROY(ts_db * self) {

    // recursivly delete db folder
    nftw(self->dir, _ts_db_delete_fs_item, 200, FTW_DEPTH);

    ts_db_close(self);
    return TS_SUCCESS;
}

int ts_db_test(ts_db * self, char * table, char * key_name) {

    // check if a value exists in a given table
    MDB_val key, val;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index
    int res = mdb_get(self->current_txn->txn, _get_dbi(self, table), &key, &val);
    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_FAILURE;
}


int ts_db_del(ts_db * self, char * table, char * key_name, char * value) {

    // delete a value from a given table
    MDB_val key, val;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index

    MDB_val to_del;
    if(value != NULL) {
        to_del.mv_size = strlen(value);
        to_del.mv_data = value;
    }
    int res = mdb_del(self->current_txn->txn, _get_dbi(self, table), &key, value == NULL ? NULL : &to_del);
    return TS_SUCCESS;
}

int ts_db_get(ts_db * self, char * table, char * key_name, MDB_val * val) {

    // get a value from a given table
    MDB_val key;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    int res = mdb_get(self->current_txn->txn, _get_dbi(self, table), &key, val);
    return res == MDB_NOTFOUND ? TS_KEY_NOT_FOUND : TS_SUCCESS;
}

int ts_db_put(ts_db * self, char * table, char * key_name, MDB_val * val) {

    // put a value into a given table
    MDB_val key;

    key.mv_size = strlen(key_name);
    key.mv_data = key_name;

    // iterate index

    int res = 1;
    res = mdb_put(self->current_txn->txn, _get_dbi(self, table), &key, val, 0);

    return res == 0 ? TS_SUCCESS : TS_FAILURE;
}


int ts_db_iter_open(ts_db_iter * self, ts_db *  db, char * table, char * name) {

    // In lmdb a key can have multiple values. This function opens a cursor
    // which can be used to iterate through the values.
    self->key.mv_size = strlen(name);
    self->key.mv_data = name;
    mdb_cursor_open(db->current_txn->txn, _get_dbi(db, table), &self->cursor);

    MDB_val empty;
    int x = mdb_cursor_get(self->cursor, &self->key, &empty, MDB_FIRST);

  return TS_SUCCESS;
}

int ts_db_iter_next(ts_db_iter * self, MDB_val * next) {
    int x = mdb_cursor_get(self->cursor, &self->key, next, MDB_GET_CURRENT);

    MDB_val empty;
    int res = mdb_cursor_get(self->cursor, &self->key, &empty, MDB_NEXT);
    if(res == MDB_NOTFOUND) return res;
    if(next->mv_size == 0) return ts_db_iter_next(self, next);
    return res;
}

int ts_db_iter_close(ts_db_iter * self) {
    mdb_cursor_close(self->cursor);
    return TS_SUCCESS;
}
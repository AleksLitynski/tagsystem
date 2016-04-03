#include "tsdoc.h"

#include <unistd.h>
#include <stdlib.h>
#include "tsutil.h"
#include "tstag.h"


void ts_doc_create(ts_env * env, ts_doc_id * id) {
    ts_util_gen_doc_id(id);

    // create 2 char folder
    // create/overwrite 38 char file
    char * docDir  = ts_util_doc_dir(env, id);
    char * docName = ts_util_doc(env, id);

    ts_util_safe_mk(docDir);
    FILE * file = fopen(docName, "ab+");

    fclose(file);
    free(docDir);
    free(docName);
}	

void ts_doc_del(ts_env * env, ts_doc_id * doc) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, val;
    MDB_cursor * cursor;

    key->mv_size = TS_KEY_SIZE_BYTES;
    key->mv_data = doc;

    // iterate index
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, "index", MDB_CREATE, &dbi);
    mdb_cursor_open(txn, dbi, &cursor);
    while (mdb_cursor_get(cursor, &key, &val, MDB_NEXT) == 0) {
        // remove each tag
        ts_tag * tag = ts_tag_create(val->mv_data);
        ts_tag_remove(env, tag, doc);
        ts_tag_close(tag);
    }

    // remove index
    mdb_delete(txn, dbi, key, val);
    mdb_cursor_close(cursor);
    mdb_txn_commit(txn);

    // delete file (and folder, if empty)
    char * docDir = ts_util_doc_dir(env, id);
    char * docName = ts_util_doc(env, id);
    unlink(docName);
    rmdir(docDir); // only deletes if the directory is empty. Perfect!
    free(docDir);
    free(docName);
    
}


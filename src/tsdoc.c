#include "tsdoc.h"
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include "../lib/sds/sds.h"
#include "../lib/fs.c/fs.h"
#include "tsid.h"
#include "tsdb.h"
#include "tserror.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

int ts_doc_create(ts_doc * self, ts_db * db) {
    self->env = db;

    // get an id
    ts_id_generate(&self->id, db);
    sds doc_id_str;
    ts_id_string(&(self->id), doc_id_str);

    // create file for the document
    sds doc_id_dir = sdsdup(doc_id_str);
    sdsrange(doc_id_dir, 0, 1);
    sds doc_dir = sdscatprintf(sdsempty(), "%s/%s", db->docs, doc_id_dir);
    sds doc_path = sdscatprintf(sdsempty(), "%s/%s", doc_dir, doc_id_str);

    fs_mkdir(doc_dir, 0700);
    fs_open(doc_path, "ab+");

    sdsfree(doc_id_dir);
    sdsfree(doc_dir);
    sdsfree(doc_path);
    
    // add entry to 'index' table
    MDB_val empty = {.mv_size = 0, .mv_data = ""};
    ts_db_put(db, "index", doc_id_str, &empty);

    return TS_SUCCESS;
}

int ts_doc_delete(ts_doc * self);

int ts_doc_open(ts_doc * self, ts_db * db, ts_id id);
int ts_doc_close(ts_doc * self);
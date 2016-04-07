#include "tsdoc.h"

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "tsutil.h"
#include "tstag.h"


void ts_doc_create(ts_env * env, ts_doc_id * id) {
    ts_util_gen_doc_id(env, id);

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
    MDB_val * key, * val;
    MDB_cursor * cursor;

    key->mv_size = TS_KEY_SIZE_BYTES;
    key->mv_data = doc;

    // iterate index
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, "index", MDB_CREATE, dbi);
    mdb_cursor_open(txn, *dbi, &cursor);
    while (mdb_cursor_get(cursor, key, val, MDB_NEXT) == 0) {
        // remove each tag
        ts_tag * tag;
        ts_tag_create(env, val->mv_data, tag);
        ts_tag_remove(env, tag, doc);
        ts_tag_close(env, tag);
    }

    // remove index
    mdb_del(txn, *dbi, key, val);
    mdb_cursor_close(cursor);
    mdb_txn_commit(txn);

    // delete file (and folder, if empty)
    char * docDir = ts_util_doc_dir(env, doc);
    char * docName = ts_util_doc(env, doc);
    unlink(docName);
    rmdir(docDir); // only deletes if the directory is empty. Perfect!
    free(docDir);
    free(docName);
    
}



void _ts_util_gen_doc_id_gen_id(ts_env * env, ts_doc_id * id);
uint8_t _ts_util_gen_doc_id_test_id(ts_env * env, ts_doc_id * id);
void ts_util_gen_doc_id(ts_env * env, ts_doc_id * id) {
    while(_ts_util_gen_doc_id_test_id(env, id)) {
        _ts_util_gen_doc_id_gen_id(env, id);
    }

}

void _ts_util_gen_doc_id_gen_id(ts_env * env, ts_doc_id * id) {
    char out[TS_KEY_SIZE_BYTES];
    // the current unix time followed by a random number
    // if time/rand is larget/smaller than 32 bits, it'll just get cut off/padded,
    // which should be ok
    uint64_t rand_id   = ((uint64_t)time(NULL) * 4) + (uint64_t)rand();
    char * rand_id_str = (char *)&rand_id;
    SHA1(rand_id_str, sizeof(uint64_t), out); 

    // in case the platform isn't char = 8 bits
    for(int i = 0; i < TS_KEY_SIZE_BYTES; i++) {
        char mask = 1;
        // if openssl filled chars from right, decriment instead
        for(int j = 0; j < 8; j++) {
            id[i] |= out[i] & mask; // take the first 8 bits of each char
            mask <<= 1;
        }
    }

}

uint8_t _ts_util_gen_doc_id_test_id(ts_env * env, ts_doc_id * id) {
    // path + folder + file
    char * fileName = ts_util_doc(env, id);
    FILE * file = fopen(fileName, "r");
    uint8_t out = file != null; 
    fclose(file);
    free(fileName);
    return out; // 1 if it exists
}


char * ts_util_doc(ts_env * env, ts_doc_id * id) {
    char * out = calloc(strlen(env->dir) + 1 + 2 + 1 + (TS_KEY_SIZE_BYTES-2) 1);
    char fileDir[3] = {0};
    fileDir[0] = id[0];
    fileDir[1] = id[1];
    sprintf(out, "%s/%s/%s", env->dir, fileDir, &id[2]);
    return out;

}
char * ts_util_doc_dir(ts_env * env, ts_doc_id * id) {
    char * out = calloc(strlen(env->dir) + 1 + 2 + 1);
    char fileDir[3] = {0};
    fileDir[0] = id[0];
    fileDir[1] = id[1];
    sprintf(out, "%s/%s", env->dir, fileDir);
    return out;
}

#include "tsdoc.h"

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "openssl/sha.h"
#include "tsutil.h"
#include "tstag.h"
#include "math.h"


void tm(char * label) {
    char * c = malloc(100000);
    printf("malloc %s\n", label);
    free(c);
}

void ts_doc_create(ts_env * env, ts_doc_id * id) {

    ts_doc_gen_id(env, id);

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

    key->mv_size = TS_ID_BYTES;
    key->mv_data = doc;

    // iterate index
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, "index", MDB_CREATE, dbi);
    mdb_cursor_open(txn, *dbi, &cursor);
    while (mdb_cursor_get(cursor, key, val, MDB_NEXT) == 0) {
        // remove each tag
        char * tag;
        ts_tag_create(env, val->mv_data);
        ts_tag_remove(env, val->mv_data, doc);
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


void _ts_doc_gen_weak_id(ts_env * env, ts_doc_id * id) {
    unsigned char out[TS_ID_BYTES];
    // the current unix time followed by a random number
    // if time/rand is larget/smaller than 32 bits, it'll just get cut off/padded,
    // which should be ok
    uint64_t rand_id = ((uint64_t)time(NULL) * 4) + (uint64_t)rand();
    const unsigned char * rand_id_str = (const unsigned char *)&rand_id;
    SHA1(rand_id_str, sizeof(uint64_t), out); 

    for(int i = 0; i < TS_ID_BYTES; i++) {
        uint8_t mask = 1;
        (*id)[i] = (uint8_t)out[i];
    }

}

uint8_t _ts_doc_test_id(ts_env * env, ts_doc_id * id) {
    // path + folder + file
    char * fileName = ts_util_doc(env, id);
    FILE * file = fopen(fileName, "r");
    uint8_t out = file != NULL; 
    if(out) fclose(file);
    free(fileName);
    return out; // 1 if it exists
}

void ts_doc_gen_id(ts_env * env, ts_doc_id * id) {
    _ts_doc_gen_weak_id(env, id);

    while(_ts_doc_test_id(env, id)) {
        _ts_doc_gen_weak_id(env, id);
    }

}



unsigned numDigits(const unsigned n, int base) {
    if (n < base) return 1;
    return 1 + numDigits(n / base, base);
}

char * ts_util_doc(ts_env * env, ts_doc_id * id) {
    char * docDir = ts_util_doc_dir(env, id);
    char * out = calloc(strlen(docDir) + 18, sizeof(char));
    sprintf(out, "%s", docDir);

    int outIdx = strlen(docDir);
    for(int i = 0; i < 18; i++) {
        sprintf(out + outIdx, "%02x", *id[2 + i]);
        outIdx += numDigits(*id[2+i], 16); 
    }
    free(docDir);
    return out;

}
char * ts_util_doc_dir(ts_env * env, ts_doc_id * id) {
    char * out;
    asprintf(&out, "%s/docs/%02x%02x/", env->dir, *id[0], *id[1]);
    return out;
}

char * ts_util_str_id(ts_doc_id * id) {
    char * out = calloc(80, sizeof(char));
    for(int i = 0; i < TS_ID_BYTES; i++) {
        sprintf(out + (i * 2), "%02x", (*id)[i]);
    }
    return out;

}


char * ts_util_str_id_bin(ts_doc_id * id) {
    char * out = calloc(TS_ID_BITS, sizeof(char));

    for(int i = 0; i < TS_ID_BITS; i++) {
        out[i] = ts_util_test_bit(*id, i) ? '1': '0';
    }

    return out;
}

char * ts_util_str_id_bin_split(ts_doc_id * id, char split, int sloc) {


    char * out = calloc(160 + 160/sloc, sizeof(char));
    int sindex = 0;
    int skips = 0;
    for(int i = 0; i < TS_ID_BITS + 20; i++) {
        out[i] = ts_util_test_bit(*id, i - skips) ? '1' : '0';
        sindex++;
        if(sindex == sloc && i+1 < TS_ID_BITS + 20) {
            out[i+1] = split;
            sindex = 0;
            skips++;
            i++;
        }
    }
    return out;
}

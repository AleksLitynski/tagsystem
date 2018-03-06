
#include "tsid.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "tserror.h"

// #define _GNU_SOURCE_
#include <stdio.h>
#include "tserror.h"

int ts_id_generate_weak(ts_id * self) {
    for(int i = 0; i < TS_ID_BYTES; i++) {
        *self[i] = (uint8_t)rand();
    }
    return TS_SUCCESS;
}

int ts_id_generate(ts_id * self, ts_db * db) {

    sds key = sdsnew("");
    sdsgrowzero(key, TS_ID_BYTES);

    ts_id_generate_weak(self);
    bool is_taken = true;
    while(is_taken) {
        // check if id is available, if it is, re-gen,
        sds idx = sdsnew("index");
        ts_id_string(self, key);
        is_taken = ts_db_test(db, idx, key) == TS_KEY_NOT_FOUND;
        if(is_taken) ts_id_generate_weak(self);
        sdsfree(idx);
    }

    sdsfree(key);
    return TS_SUCCESS;
}

int ts_id_string(ts_id * self, sds str) {

    for(int i = 0; i < TS_ID_BYTES; i++) {
        sprintf(str + (i * 2), "%02x", (*self)[i]);
    }

    return TS_SUCCESS;
}

bool ts_id_eq(ts_id * self, ts_id * other) {
    bool eq = true;
    for(int i = 0; i < TS_ID_BYTES; i++) {
        if(self[i] != other[i]) {
          eq = false;
          break;
        }
    }
    return eq;
}

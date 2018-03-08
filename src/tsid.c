
#include "tsid.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tserror.h"

// #define _GNU_SOURCE_
#include <stdio.h>
#include "tserror.h"

int ts_id_generate_weak(ts_id * self) {
    
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*self)[i] = (uint8_t)rand();
    }
    return TS_SUCCESS;
}

int ts_id_generate(ts_id * self, ts_db * db) {

    ts_id_generate_weak(self);
    bool is_taken = true;
    while(is_taken) {
        // check if id is available, if it is, re-gen,
        sds idx = sdsnew("index");
        sds key = ts_id_string(self, sdsempty());
        is_taken = ts_db_test(db, idx, key) != TS_KEY_NOT_FOUND;
        if(is_taken) ts_id_generate_weak(self);
        sdsfree(idx);
        sdsfree(key);
    }

    return TS_SUCCESS;
}

sds ts_id_string(ts_id * self, sds str) {

    for(int i = 0; i < TS_ID_BYTES; i++) {
        // unsigned int, max 3 digits, pad with 0s if less than 3
        str = sdscatprintf(str, "%03u", (*self)[i]);
    }

    return str;
}

bool ts_id_eq(ts_id * self, ts_id * other) {
    bool eq = true;
    for(int i = 0; i < TS_ID_BYTES; i++) {
        if((*self)[i] != (*other)[i]) {
          eq = false;
          break;
        }
    }
    return eq;
}

int ts_id_dup(ts_id * self, ts_id * other) {
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*other)[i] = (*self)[i];
    }
    return TS_SUCCESS;
}

int ts_id_value(ts_id * self, int index) {
    return ((*self)[index/8] & (((uint8_t)1) << ( 7 - (index % 8)))) > 0;
}


int ts_id_from_string(ts_id * self, sds str) {
    for(int i = 0; i < TS_ID_BYTES; i++) {
        sds str_i = sdsdup(str);
        sdsrange(str_i, i * 3, (i * 3) + 2);
        (*self)[i] = atoi(str_i);
        sdsfree(str_i);
    }
}

#include "tsid.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tserror.h"

// #define _GNU_SOURCE_
#include <stdio.h>
#include "tserror.h"

int ts_id_generate_weak(ts_id * self) {
    // generate a random id that may or may not be unique
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*self)[i] = (uint8_t)rand();
    }
    return TS_SUCCESS;
}

int ts_id_generate(ts_id * self, ts_db * db) {
    // continually generate weak ids until an available id is found 
    ts_id_generate_weak(self);
    bool is_taken = true;
    while(is_taken) {
        // check if id is available, if it is, re-gen,
        sds key = ts_id_string(self, sdsempty());
        is_taken = ts_db_test(db, "index", key) != TS_KEY_NOT_FOUND;
        if(is_taken) ts_id_generate_weak(self);
        sdsfree(key);
    }

    return TS_SUCCESS;
}


int ts_id_empty(ts_id * self) {
    // generate an empty id
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*self)[i] = 0;
    }
    return TS_SUCCESS;
}

sds ts_id_string(ts_id * self, sds str) {
    // print the id to a decimal string
    for(int i = 0; i < TS_ID_BYTES; i++) {
        // unsigned int, max 3 digits, pad with 0s if less than 3
        str = sdscatprintf(str, "%03u", (*self)[i]);
    }

    return str;
}

sds ts_id_bit_string(ts_id * self, sds str) {
    // print the id to a binary string
    return ts_id_nbit_string(self, str, TS_ID_BITS);
}

sds ts_id_nbit_string(ts_id * self, sds str, int nbits) {
    for(int i = 0; i < nbits; i++) {
        // unsigned int, max 3 digits, pad with 0s if less than 3
        str = sdscatprintf(str, "%i", ts_id_get_bit(self, i));
    }

    return str;
}

bool ts_id_eq(ts_id * self, ts_id * other) {
    // check if two ids are equal.
    bool eq = true;
    // should be safe to iterate per byte instead of per bit
    for(int i = 0; i < TS_ID_BYTES; i++) {
        if((*self)[i] != (*other)[i]) {
          eq = false;
          break;
        }
    }
    return eq;
}

int ts_id_dup(ts_id * self, ts_id * other) {
    // copy an id
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*other)[i] = (*self)[i];
    }
    return TS_SUCCESS;
}

int ts_id_get_bit(ts_id * self, int index) {
    // gets a bit an a given index of an id
    return ((*self)[index/8] & (((uint8_t)1) << ( 7 - (index % 8)))) > 0;
}

int ts_id_set_bit(ts_id * self, int index, int bit) {
    // sets a bit at a given index of an id
    int block = index / 8;
    int remainder = 7 - (index % 8);
    if(bit == 1) {
        (*self)[block] |= 1 << remainder;
    }
    if(bit == 0) {
        (*self)[block] &= ~(1 << remainder);
    }
    return TS_SUCCESS;
}

int ts_id_from_string(ts_id * self, sds str) {
    // converts a decimal string to an id
    int byte_index = 0;
    for(int i = 0; i < TS_ID_BYTES; i++) {
        char next[3] = { str[byte_index], str[byte_index + 1], str[byte_index + 2] };
        (*self)[i] = atoi(next);
        byte_index += 3;
    }
}
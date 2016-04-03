#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lib/openssl/include/sha.h"


char * ts_util_concat(char * a, char * b) {
    int l1 = strlen(a);
    int l2 = strlen(b);
    char * c = malloc (l1 + l2 + 1);
    memcpy(c, a, l1);
    memcpy(c + l1, b, l2 + 1);
    return c;
}

void ts_util_safe_mk(char * name) {
    struct stat st = {0};
    if(stat(name, &st) == -1) {
        mkdir(name, 0700);
    }
}

void ts_util_gen_doc_id(ts_env * env, ts_doc_id * id) {
    while(_ts_util_gen_doc_id_test_id(env, id)) {
        _ts_util_gen_doc_id_gen_id(env, id);
    }

}

void _ts_util_gen_doc_id_gen_id(ts_env * env, ts_doc_id * id) {
    char out[TS_KEY_SIZE_BYTES];
    char * id_src = ts_util_concat(time(NULL), rand());
    SHA1(id_src, strlen(id_src), out); 

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

uint8_t ts_util_test_bit(uint8_t * firstByte, unsigned int index) {
    unsigned int indexByte = targetIndex / 8;
    unsigned int indexBit = targetIndex % 8;
    return firstByte[indexByte] & (1 << indexBit);
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

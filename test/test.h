#pragma once

// includes
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <time.h>
#include "cmocka.h"
#include "sds.h"
#include "fs.h"
#include "tsdb.h"
#include "tsid.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tstags.h"
#include "tssearch.h"

// macros
#define LOG(fmt, ...) printf ("[ INFO     ] " fmt "\n", __VA_ARGS__)
#define LOG1(fmt) LOG(fmt, "")
#define LOGID(id) {                         \
    sds str = ts_id_string(id, sdsempty()); \
    LOG("    %s", str);                     \
    sdsfree(str);                           \
}
#define LOGIDBIN(id) {                          \
    sds str = ts_id_bit_string(id, sdsempty()); \
    LOG("    %s", str);                         \
    sdsfree(str);                               \
}


#define LOGTAGS(tags) {                                             \
    sds str = ts_tags_print(tags, sdsempty());                      \
    int count;                                                      \
    sds * lines = sdssplitlen(str, sdslen(str), "\n", 1, &count);   \
    for(int i = 0; i < count; i++) {                                \
        LOG("%s", lines[i]);                                        \
    }                                                               \
    sdsfreesplitres(lines, count);                                  \
}

#define ID(binary) \
    ts_id id_ ## binary = {0}; \
    id_from_binary_string(#binary, &id_ ## binary);


// types
typedef struct {
    ts_db * db;
} test_state;

// functions
// main
void set_id(ts_id * id, uint8_t value);
static int setup(void **state);
static int teardown(void **state);
void id_from_binary_string(char * source, ts_id * id);
void id_from_binary_string_tail(char * source, ts_id * id);

// docs
void doc_test(void ** state);

// tags
void tags_test(void ** state);
void tag_empty_test(void ** state);
void tag_insert_test(void ** state);
void tag_remove_test(void ** state);
void tag_shuffle_test(void ** state);
void tag_double_ops_test(void ** state);

// ids
void id_test(void ** state);
void id_value_test(void ** state);
void id_to_str_test(void ** state);

// search
void search_test(void ** state);
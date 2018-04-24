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
#include "hash.h"
#include "tsdb.h"
#include "tsid.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tstags.h"
#include "tssearch.h"

// utils
#include "tsargs.h"
#include "tscli.h"
#include "tscliutils.h"
#include "tssearchset.h"
#include "tsstr.h"
#include "tstaglist.h"
#include "tstagset.h"
#include "tsclihelp.h"

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


#define LOGTAGS(tags) ts_tags_log(tags)

#define ID(binary) \
    ts_id id_ ## binary = {0}; \
    id_from_binary_string(#binary, &id_ ## binary);

// replace memory allocation functions with test functions that log on forgotten frees and memory 'coruption'va
#define calloc(a, b) test_calloc(a, b)
#define free(a) test_free(a);
#define malloc(a) test_malloc(a)
#define realloc(a, b) test_realloc(a, b)

// types
typedef struct {
    ts_db * db;
    ts_cli_ctx * ctx;
    FILE * write_input;
    FILE * read_output;
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
void doc_unique_test(void ** state);

// tags
void tags_test(void ** state);
void tag_empty_test(void ** state);
void tag_insert_test(void ** state);
void tag_remove_test(void ** state);
void tag_shuffle_test(void ** state);
void tag_double_ops_test(void ** state);
void tag_mdb_test(void ** state);

// ids
void id_value_test(void ** state);
void id_to_str_test(void ** state);

// search
void search_test(void ** state);
void search_remove_test(void ** state);
void search_intersection_test(void ** state);

// parsing
void parse_tags_test(void ** state);
void string_processing_test(void ** state);
void arg_parsing_test(void ** state);

// cli command tests
void cli_makeremove_test(void ** state);
void cli_changeset_test(void ** state);
void cli_help_test(void ** state);
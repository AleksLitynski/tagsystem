#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include "sds.h"
#include "fs.h"
#include "tsdb.h"
#include "tsid.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tstags.h"

#define LOG(fmt, ...) printf ("[ +LOG     ] " fmt "\n", __VA_ARGS__)
#define LOG1(fmt) LOG(fmt, "")
#define LOGID(id) {                             \
        sds str = ts_id_string(id, sdsempty()); \
        LOG("    %s", str);                     \
        sdsfree(str); }

typedef struct {
    ts_db * db;
} test_state;

void set_id(ts_id * id, uint8_t value) {
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*id)[i] = value;
    }
}

static int setup(void **state) {
    test_state * st = malloc(sizeof(test_state));
    st->db = malloc(sizeof(ts_db));

    ts_db_open(st->db, "test_db");

    *state = st;
    return 0;
}

static int teardown(void **state) {
    test_state * st = (test_state*)*state;
    // ts_db_close(st->db);
    ts_db_DESTROY(st->db);
    free(*state);

    return 0;
}


void id_test(void ** state) {
    test_state * st = (test_state*)*state;

    int items = 3;
    ts_id ids[items];
    
    for(int i = 0; i < items; i++) {
      ts_id_generate(&ids[i], st->db);
    }
    
    int duplicates = 0;
    for(int i = 0; i < items; i++) {
        for(int j = 0; j < items; j++) {
            if(ts_id_eq(&ids[i], &ids[j])) {
                duplicates++;
            }
        }

        LOGID(&ids[i]);
    }

    assert_true(duplicates == items);
}

void id_value_test(void ** state) {

    ts_id sample_1;
    set_id(&sample_1, 255);
    LOGID(&sample_1);
    for(int i = 0; i < TS_ID_BYTES; i++) { 
        assert_int_equal(ts_id_value(&sample_1, i), 1);
    }
    
    ts_id sample_0;
    set_id(&sample_0, 0);
    LOGID(&sample_0);
    for(int i = 0; i < TS_ID_BYTES; i++) { 
        assert_int_equal(ts_id_value(&sample_0, i), 0);
    }
    
    ts_id sample_1010;
    set_id(&sample_1010, 170);
    LOGID(&sample_1010);
    for(int i = 0; i < TS_ID_BYTES; i++) {
        int v = ts_id_value(&sample_1010, i);
        assert_int_equal(v, !(i % 2));
    }
}

void id_to_str_test(void ** state) {
    ts_id sample;
    set_id(&sample, 170);
    sds sample_str = ts_id_string(&sample, sdsempty());
    ts_id sample_from_str;
    ts_id_from_string(&sample_from_str, sample_str);

    for(int i = 0; i < TS_ID_BYTES; i++) {
        assert_int_equal(sample[i], sample_from_str[i]);
    }
    sdsfree(sample_str);
}

void doc_test(void ** state) {
    test_state * st = (test_state*)*state;

    char * contents, * read;
    contents = "some text";
    ts_doc doc, doc2;
    ts_id doc_id;

    // create a document and write some text to it
    ts_doc_create(&doc, st->db);
    ts_id_dup(&doc.id, &doc_id);
    fs_write (doc.path, contents);
    // close the document
    ts_doc_close(&doc);

    // reopen the document and read the text
    ts_doc_open(&doc2, st->db, doc_id);
    read = fs_read(doc2.path);
    ts_doc_delete(&doc2);

    // confirm the index was deleted
    sds idx = sdsnew("index");
    sds val = ts_id_string(&doc_id, sdsempty());
    bool delete_success = ts_db_test(st->db, idx, val) == TS_KEY_NOT_FOUND;
    sdsfree(idx);
    sdsfree(val);

    // confirm the document had the correct text in it
    assert_string_equal(contents, read);
    assert_true(delete_success);
    free(read);
}

void tags_test(void ** state) {
    test_state * st = (test_state*)*state;

    ts_tags tags;
    ts_tags_empty(&tags);

    
    for(int i = 0; i < 3; i++) {
        ts_id id;
        ts_id_generate(&id, st->db);
        ts_tags_insert(&tags, &id);
    }

    // int ts_tags_remove(ts_tags * self, ts_id * id);

    sds ts_tags_print(ts_tags * self, sds printed);
    sds tags_str = ts_tags_print(&tags, sdsempty());
    LOG("\n%s\n", tags_str);
    sdsfree(tags_str);
    ts_tags_close(&tags);

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(id_test),
        cmocka_unit_test(id_value_test),
        cmocka_unit_test(doc_test),
        cmocka_unit_test(id_to_str_test),
        cmocka_unit_test(tags_test),
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}

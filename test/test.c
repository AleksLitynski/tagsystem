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

#define LOG(fmt, ...) printf ("[ +LOG     ] " fmt "\n", __VA_ARGS__)
#define LOG1(fmt) LOG(fmt, "")

typedef struct {
    ts_db * db;
} test_state;

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
    LOG1("id_test a");
    test_state * st = (test_state*)*state;
    LOG1("id_test b");
    int items = 10;
    ts_id ids[items];
    LOG1("id_test c");
    for(int i = 0; i < items; i++) {
      LOG1("id_test d");
      ts_id_generate(&ids[i], st->db);
      LOG1("id_test e");
    }
    LOG1("id_test f");
    int duplicates = 0;
    sds str;
    for(int i = 0; i < items; i++) {
        for(int j = 0; j < items; j++) {
            if(ts_id_eq(&ids[i], &ids[j])) {
                duplicates++;
            }
        }
        ts_id_string(&ids[i], str);
        LOG("    %s", str);
    }
    sdsfree(str);

    LOG1("id_test c");
    assert_true(duplicates == 0);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(id_test),
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}

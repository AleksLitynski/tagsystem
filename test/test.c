#include "test.h"


// utilities for working with ids
void set_id(ts_id * id, uint8_t value) {
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (*id)[i] = value;
    }
}

void id_from_binary_string(char * source, ts_id * id) {
    int len = strlen(source);
    for(int i = 0; i < TS_ID_BITS; i++) {
        if(i < len) {
            ts_id_set_bit(id, i, source[i] == '1' ? 1 : 0);
        }
    }
}

void id_from_binary_string_tail(char * source, ts_id * id) {
    int len = strlen(source);
    for(int i = TS_ID_BITS - 1; i >= 0; i--) {
        if(i >= len) {
            ts_id_set_bit(id, i, source[i] == '1' ? 1 : 0);
        }
    }
}

static int setup(void **state) {

    // when the test suite starts, a new database is created
    test_state * st = malloc(sizeof(test_state));
    st->db = malloc(sizeof(ts_db));

    ts_db_open(st->db, "test_db");
    // wipe the old db
    ts_db_DESTROY(st->db);
    ts_db_open(st->db, "test_db");

    st->ctx = malloc(sizeof(ts_cli_ctx));
    st->ctx->db = st->db;

    // create files to represent stdin and stdout
    // helpful when testing cli functions
    st->write_input = fopen("test_db/io_in", "w");
    st->ctx->in = fopen("test_db/io_in", "r");

    st->ctx->out = fopen("test_db/io_out", "w");
    st->read_output = fopen("test_db/io_out", "r");

    *state = st;
    return 0;
}

static int teardown(void **state) {
    test_state * st = (test_state*)*state;

    fclose(st->ctx->in);
    fclose(st->ctx->out);
    fclose(st->write_input);
    fclose(st->read_output);
    free(st->ctx);

    // ts_db_close(st->db);
    ts_db_DESTROY(st->db);
    free(st->db);
    free(*state);

    return 0;
}



int main(void) {
    const struct CMUnitTest tests[] = {
        // cmocka_unit_test(id_value_test),
        // cmocka_unit_test(id_to_str_test),

        // cmocka_unit_test(doc_test),
        // cmocka_unit_test(doc_unique_test),
        
        // cmocka_unit_test(tags_test),
        // cmocka_unit_test(tag_empty_test),
        // cmocka_unit_test(tag_insert_test),
        // cmocka_unit_test(tag_remove_test),
        // cmocka_unit_test(tag_shuffle_test),
        // cmocka_unit_test(tag_double_ops_test),
        // cmocka_unit_test(tag_mdb_test),

        // cmocka_unit_test(search_test),
        // cmocka_unit_test(search_remove_test),
        // cmocka_unit_test(search_intersection_test),

        // cmocka_unit_test(parse_tags_test),
        // cmocka_unit_test(string_processing_test),
        cmocka_unit_test(arg_parsing_test),

        // cmocka_unit_test(cli_makeremove_test),
        // cmocka_unit_test(cli_changeset_test),
        // cmocka_unit_test(cli_doubleinsert_test),
        // cmocka_unit_test(cli_help_test)

    };

    return cmocka_run_group_tests_name("all tests", tests, setup, teardown);

}
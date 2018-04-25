#include "test.h"


void read_output(test_state * st, char * out, int max_size) {
    fflush(st->ctx->out);
    fgets(out, 1000, st->read_output);
    fflush(st->read_output);
}

void write_input(test_state * st, char * in) {
    fflush(st->ctx->in);
    fprintf(st->write_input, "%s", in);
    fflush(st->write_input);
}

void cli_makeremove_test(void ** state) {
    test_state * st = (test_state*)*state;

    
    // create doc and read id
    CLI(st->ctx, ts_cli_make, "-pi", "tag_name");

    char new_id[1000] = {0};
    read_output(st, new_id, 1000);

    // apply tag
    write_input(st, new_id);
    CLI(st->ctx, ts_cli_tag, "--silent", "new_tag_name");
    CLI(st->ctx, ts_cli_list, "--id", "tag_name");

    char tagged_id[1000] = {0};
    read_output(st, tagged_id, 1000);

    // delete the new doc
    CLI(st->ctx, ts_cli_remove, "-f", "-s", "new_tag_name");
    CLI(st->ctx, ts_cli_list, "--id", "-t", "new_tag_name + tag_name");

    char removed_id[1000] = {0};
    read_output(st, removed_id, 1000);

    // confirm new_id, and tagged_id are the same
    assert_string_equal(new_id, tagged_id);

    // confirm removed_id is empty
    assert_string_equal("", removed_id);

}

void cli_changeset_test(void ** state) {
    test_state * st = (test_state*)*state;

    char current_set[1000] = {0};

    // can't write to stdin, so nothing's really being tagged
    CLI(st->ctx, ts_cli_changeset, "-s", "~~");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "-s", "+a");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("+a\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "-s", "~a");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("\n", current_set);


    CLI(st->ctx, ts_cli_changeset, "-s", "+a +b");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("+a+b\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "-s", "~~");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "-s", "+b");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("+b\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "-s", "zebra");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("+b+zebra\n", current_set);

    CLI(st->ctx, ts_cli_changeset, "~~");
    CLI(st->ctx, ts_cli_presentset, "");
    read_output(st, current_set, 1000);
    assert_string_equal("\n", current_set);

}

void cli_doubleinsert_test(void ** state) {
    test_state * st = (test_state*)*state;

    CLI(st->ctx, ts_cli_changeset, "+aa +b");
    CLI(st->ctx, ts_cli_changeset, "+b");


}

void cli_help_test(void ** state) {
    test_state * st = (test_state*)*state;
    
    CLI(st->ctx, ts_cli_help, "");
}

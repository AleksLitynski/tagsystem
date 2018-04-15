#include "test.h"


// char * argv[] = {"-f", "new_tag_name"};
// ts_searchset_create(st->ctx);
// ts_args args;
// ts_args_create(&args);
// ts_args_parse(&args, 2, argv);

// ts_tags * tags = malloc(sizeof(ts_tags));

// hash_each(st->ctx->pws, {
//     ts_tags_open(tags, st->ctx->db, key);
//     ts_tags_log(tags);
//     break;
// })

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
    
    // // create doc and read id
    CLI(st->ctx, ts_cli_make, "-pi", "tag_name");

    char new_id[1000];
    read_output(st, new_id, 1000);

    // apply tag
    write_input(st, new_id);
    CLI(st->ctx, ts_cli_tag, "new_tag_name");
    CLI(st->ctx, ts_cli_list, "--id", "tag_name");

    char tagged_id[1000];
    read_output(st, tagged_id, 1000);

    // delete the new doc
    CLI(st->ctx, ts_cli_remove, "-f", "new_tag_name");
    CLI(st->ctx, ts_cli_list, "--id", "new_tag_name + tag_name");

    char removed_id[1000];
    read_output(st, removed_id, 1000);

    // confirm new_id, and tagged_id are the same
    assert_string_equal(new_id, tagged_id);

    // confirm removed_id is empty
    assert_string_equal("", removed_id);

}

void cli_changeset_test(void ** state) {
    test_state * st = (test_state*)*state;

    // can't write to stdin, so nothing's really being tagged
    CLI(st->ctx, ts_cli_changeset, "+added_to_set ~removed_from_set");
    CLI(st->ctx, ts_cli_presentset, "");
}

void cli_help_test(void ** state) {
    test_state * st = (test_state*)*state;
    
    CLI(st->ctx, ts_cli_help, "");
}

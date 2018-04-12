#include "test.h"


void parse_tags_test(void ** state) {

    hash_t * tags = hash_new();
    ts_tagset_create(tags, "+a   b+basd~basd~~      r  a+nope");

    // hash_each(tags, {
    //     LOG("%s", key);
    // })

    assert_string_equal("r  a", (char *) hash_get(tags, "r  a"));
    assert_string_equal("nope", (char *) hash_get(tags, "nope"));

    ts_tagset_close(tags);

}


void string_processing_test(void ** state) {

    // string operations
    sds lower_case = sdsnew("AaAb1212@!$()De fGH");
    ts_str_to_lower(lower_case);
    assert_string_equal(lower_case, "aaab1212@!$()de fgh");
    sdsfree(lower_case);

    sds without = ts_str_without_vowels(sdsempty(), "carterisnvoqwe u");
    assert_string_equal(without, "crtrsnvqw ");
    sdsfree(without);
    
    assert_true(ts_str_begins_with("abc", "ab"));
    assert_true(ts_str_begins_with("-f", "-"));
    assert_true(ts_str_begins_with("--force", "--"));

    assert_true(ts_args_matches("force", "fR"));
    assert_true(ts_args_matches("force", "f"));
    assert_true(ts_args_matches("force", "forcE"));

}

void arg_parsing_test(void ** state) {

    ts_args args;
    ts_args_create(&args, 6);

    bool ** a = ts_args_add_bool(&args, "aaaa");
    bool ** b = ts_args_add_bool(&args, "bbbb");
    bool ** e = ts_args_add_bool(&args, "ee");
    char ** c = ts_args_add_str(&args, "cccc");
    char ** d = ts_args_add_str(&args, "dddd");
    bool ** force = ts_args_add_bool(&args, "force");

    char * chars[] = {
        "--aaaa",
        "-c",
        "c_value",
        "-eb",
        "--ddd",
        "d_value",
        "__rest__",
        "__rest__2"
    };


    int unused = ts_args_parse(&args, sizeof(chars) / sizeof(char *), chars);

    assert_true(**a);
    assert_true(**b);
    assert_true(**c);
    assert_false(**force);

    assert_string_equal(*c, "c_value");
    assert_string_equal(*d, "d_value");


    ts_args_close(&args);
}
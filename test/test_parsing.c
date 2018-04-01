#include "test.h"


void parse_tags_test(void ** state) {

    hash_t * tags = hash_new();
    tag_set(tags, "+a   b+basd~basd~~      r  a+nope");

    // hash_each(tags, {
    //     LOG("%s", key);
    // })

    assert_string_equal("r  a", (char *) hash_get(tags, "r  a"));
    assert_string_equal("nope", (char *) hash_get(tags, "nope"));

    tag_set_free(tags);

}


void string_processing_test(void ** state) {

    // string operations
    sds lower_case = sdsnew("AaAb1212@!$()De fGH");
    to_lower(lower_case);
    assert_string_equal(lower_case, "aaab1212@!$()de fgh");
    sdsfree(lower_case);

    sds without = without_vowels(sdsempty(), "carterisnvoqwe u");
    assert_string_equal(without, "crtrsnvqw ");
    sdsfree(without);
    
    assert_true(begins_with("abc", "ab"));
    assert_true(begins_with("-f", "-"));
    assert_true(begins_with("--force", "--"));

    assert_true(matches("force", "f"));
    assert_true(matches("force", "fR"));
    assert_true(matches("force", "forcE"));

}

void arg_parsing_test(void ** state) {

    arg_list args;
    args_create(&args, 6);

    bool ** a = args_add_bool(&args, "aaaa");
    bool ** b = args_add_bool(&args, "bbbb");
    bool ** e = args_add_bool(&args, "ee");
    char ** c = args_add_str(&args, "cccc");
    char ** d = args_add_str(&args, "dddd");
    bool ** force = args_add_bool(&args, "force");

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


    int unused = args_parse(&args, sizeof(chars) / sizeof(char *), chars);

    assert_true(**a);
    assert_true(**b);
    assert_true(**c);
    assert_false(**force);

    assert_string_equal(*c, "c_value");
    assert_string_equal(*d, "d_value");


    args_close(&args);
}
#include "test.h"


void parse_tags_test(void ** state) {

    hash_t * tags = hash_new();
    tag_set(tags, "+a   b+basd~basd~~      r  a+nope");

    hash_each(tags, {
        LOG("%s", key);
    })

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
    args_create(&args, 4);

    bool * a = args_add_bool(&args, "aaaa");
    bool * b = args_add_bool(&args, "bbbb");
    char * c = args_add_str(&args, "cccc");
    char * d = args_add_str(&args, "dddd");


    char * chars[4];
    chars[0] = "a";
    chars[1] = "b";
    chars[2] = "c";
    chars[3] = "d";

    int last_taken = args_parse(&args, 4, chars);


    int args_close(arg_list * self);
}
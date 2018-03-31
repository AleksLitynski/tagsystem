#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros
#define TS_CTX_ADD_TAG '+'
#define TS_CTX_DEL_TAG '~'

// types
typedef enum {
    ARG_TYPE_BOOL,
    ARG_TYPE_STR
} arg_type;

typedef struct {
    arg_type type;
    char * name;
    void * value;
} arg;

typedef struct {
    int size;
    int next;
    bool pending_value;
    void ** pending_value_addr;
    arg * args;
} arg_list;

const bool _arg_true_const;

// functions

// tag set operations
sds _push_tag(hash_t * tags, sds next_tag, char mode);
int tag_set(hash_t * tags, sds tag_str);
int tag_set_free(hash_t * tags);

// string operations
int to_lower(char * self);
bool is_vowel(char x);
sds without_vowels(sds without, char * src);
bool begins_with(char * src, char * prefix);

// cli argument parsing
bool matches(char * arg_name, char * arg_input_value);

int args_create(arg_list * self, int size);
bool ** args_add_bool(arg_list * self, char * name);
char ** args_add_str(arg_list * self, char * name);
int args_parse(arg_list * self, int argc, char * argv[]);
int args_close(arg_list * self);
void ** _args_add(arg_list * self, char * name, arg_type type);
bool _arg_set_param(arg_list * self, char * arg_input_value);

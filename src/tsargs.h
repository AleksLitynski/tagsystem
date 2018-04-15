#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros

// types
typedef enum {
    ARG_TYPE_BOOL,
    ARG_TYPE_STR
} ts_arg_type;

typedef struct ts_arg {
    ts_arg_type type;
    char * name;
    void * value;

    struct ts_arg * next;
} ts_arg;

typedef struct {
    bool pending_value;
    void ** pending_value_addr;
    ts_arg * latest_arg;
    ts_arg * args;
    sds rest;
} ts_args;

const bool _ts_args_true;
const bool _ts_args_false;

// functions

// tag set operations
bool ts_args_matches(char * arg_name, char * arg_input_value);

int ts_args_create(ts_args * self);
bool ** ts_args_add_bool(ts_args * self, char * name);
char ** ts_args_add_str(ts_args * self, char * name);
int ts_args_parse(ts_args * self, int argc, char * argv[]);
int _ts_arg_close(ts_arg * arg);
int ts_args_close(ts_args * self);

void ** _ts_args_add(ts_args * self, char * name, ts_arg_type type);
bool _ts_args_set_param(ts_args * self, char * arg_input_value);
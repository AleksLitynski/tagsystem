#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "hash.h"
#include "sds.h"
#include "tserror.h"
#include "tsargs.h"
#include "tsstr.h"

const bool _ts_cli_args_true = true;
const bool _ts_cli_args_false = false;

bool ts_args_matches(char * arg_name, char * arg_input_value) {

    bool success = false;

    char * op_lowercase = malloc(strlen(arg_input_value) + 1);
    strcpy(op_lowercase, arg_input_value);
    ts_str_to_lower(op_lowercase);

    sds no_vowels = ts_str_without_vowels(sdsempty(), arg_name);

    if(ts_str_begins_with(arg_name, op_lowercase)) success = true;
    if(ts_str_begins_with(no_vowels, op_lowercase)) success = true;

    free(op_lowercase);
    sdsfree(no_vowels);

    return success;
}

int ts_args_create(ts_args * self, int size) {
    self->size = size;
    self->next = 0;
    self->pending_value = false;
    self->args = malloc(sizeof(ts_arg) * size);
    return TS_SUCCESS;
}

void ** _ts_args_add(ts_args * self, char * name, ts_arg_type type) {

    self->args[self->next].type = type;
    self->args[self->next].name = name;
    void ** out = &self->args[self->next].value;

    self->next++;
    return out;
}


bool ** ts_args_add_bool(ts_args * self, char * name) {
    bool ** out = (bool**)_ts_args_add(self, name, ARG_TYPE_BOOL);
    *out = &_ts_cli_args_false;
    return out;
}

char ** ts_args_add_str(ts_args * self, char * name) {
    return (char**)_ts_args_add(self, name, ARG_TYPE_STR);
}

bool _ts_args_set_param(ts_args * self, char * arg_input_value) {
    bool success = false;

    for(int j = 0; j < self->size; j++) {
        if(ts_args_matches(self->args[j].name, arg_input_value)) {
            if(self->args[j].type == ARG_TYPE_BOOL) {
                self->args[j].value = (void*)(&_ts_args_true);
                self->pending_value = false;
                success = true;
                break;
            } else if(self->args[j].type == ARG_TYPE_STR) {
                self->pending_value_addr = &self->args[j].value;
                self->pending_value = true;
                success = true;
                break;
            }
        }
    }

    return success;
}

int ts_args_parse(ts_args * self, int argc, char * argv[]) {

    self->pending_value = false;

    int i = 0;
    for(; i < argc; i++) {
        
        if(ts_str_begins_with(argv[i], "--")) {
            sds next = sdsnew(argv[i]);
            sdstrim(next, "-");
            bool success = _ts_args_set_param(self, next);
            sdsfree(next);

            if(!success) {
                // error parsing argument
                break;
            }
        }

        else if(ts_str_begins_with(argv[i], "-")) {

            // arguments can be packed, so each char is a seperate arg
            int argv_len = strlen(argv[i]);
            for(int j = 1; j < argv_len; j++) {
                char next_char[2];
                next_char[0] = argv[i][j];
                next_char[1] = 0;

                if(!_ts_args_set_param(self, next_char)) {
                    // parsing error
                    break;
                }
            }
        }

        // if we're waiting for a string argument
        else if(self->pending_value) {
            *self->pending_value_addr = argv[i];
            self->pending_value = false;

        } 
        
        // otherwise, we're out of well formatted arguments
        else {
            break;
        }
    }

    // concat remaining values into a single 'rest' string
    self->rest = ts_str_concat_string(sdsempty(), argc - i, &argv[i]);

    return TS_SUCCESS;
}

int ts_args_close(ts_args * self) {
    sdsfree(self->rest);
    free(self->args);
    return TS_SUCCESS;
}
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "hash.h"
#include "sds.h"
#include "tserror.h"
#include "tsargs.h"
#include "tsstr.h"

// boolean arguments will be set to pointers to one of these values
const bool _ts_args_true = true;
const bool _ts_args_false = false;


bool ts_args_matches(char * arg_name, char * arg_input_value) {
    /*
        checks if an argument matches a given input following certain rules:
        1. case is ignored
        2. an input can only be the first few letters of an argument (ie lis will match list)
        3. vowels will be ignored (ie ls will match list)
    */

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

int ts_args_create(ts_args * self) {
    // create an argument parsing object
    self->pending_value = false;
    self->latest_arg = 0;
    self->args = 0;
    return TS_SUCCESS;
}

void ** _ts_args_add(ts_args * self, char * name, ts_arg_type type) {
    /* add an argument to the 'self' argument parsing object
    arguments are stored as a singly linked list */

    ts_arg * arg = calloc(sizeof(ts_arg), 1);

    if(self->args != 0) {
        self->latest_arg->next = arg;
        self->latest_arg = arg;
    } else {
        self->args = arg;
        self->latest_arg = arg;
    }

    self->latest_arg->type = type;
    self->latest_arg->name = name;
    
    return &self->latest_arg->value;
}

bool ** ts_args_add_bool(ts_args * self, char * name) {
    bool ** out = (bool**)_ts_args_add(self, name, ARG_TYPE_BOOL);
    *out = &_ts_args_false;
    return out;
}

char ** ts_args_add_str(ts_args * self, char * name) {
    return (char**)_ts_args_add(self, name, ARG_TYPE_STR);
}

bool _ts_args_set_param(ts_args * self, char * arg_input_value) {
    /* once an argument has been found, this function sets the value
    or flags that we are waiting for a string value to bind to the
    argument name */

    LOG("Matching arg: %s", arg_input_value);

    bool success = false;

    ts_arg * arg = self->args;

    while(arg != 0) {
        LOG("Matching against: %s", arg_input_value);
        if(ts_args_matches(arg->name, arg_input_value)) {
            if(arg->type == ARG_TYPE_BOOL) {
                LOG1("Matched as bool");
                arg->value = (void*)(&_ts_args_true);
                self->pending_value = false;
                success = true;
                break;
            } else if(arg->type == ARG_TYPE_STR) {
                LOG1("Matched as str");
                self->pending_value_addr = &arg->value;
                self->pending_value = true;
                success = true;
                break;
            }
        }

        arg = arg->next; 
    }

    return success;
}

int ts_args_parse(ts_args * self, int argc, char * argv[]) {
    /* Go through each provided argument and bind it to 
    the argument variable that corosponds to its name */

    self->pending_value = false;

    LOG1("Entered ts_args_parse");

    int i = 0;
    for(; i < argc; i++) {
        LOG("Now parsing arg: %s", argv[i]);
        
        if(ts_str_begins_with(argv[i], "--")) {
            LOG1("Found --");
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
            LOG1("Found -");

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
            LOG1("Found other");
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


    ts_arg * current = self->args;
    LOG1("Discovered Args:");
    while(current != 0) {
        if(current->type == ARG_TYPE_BOOL) {
            LOG("   %s -> %d", current->name, (bool)current->value);

        } else if(current->type == ARG_TYPE_STR) {
            LOG("   %s -> %s", current->name, (char *)current->value);
        }
        
        current = current->next;
    }


    return TS_SUCCESS;
}

int _ts_arg_close(ts_arg * arg) {
    // unwind the linked list of arguments
    if(arg->next != 0) _ts_arg_close(arg->next);
    free(arg);
    return TS_SUCCESS;
}

int ts_args_close(ts_args * self) {
    sdsfree(self->rest);
    _ts_arg_close(self->args);
    return TS_SUCCESS;
}
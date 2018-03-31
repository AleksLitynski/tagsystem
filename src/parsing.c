#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "hash.h"
#include "sds.h"
#include "tserror.h"
#include "parsing.h"

sds _push_tag(hash_t * tags, sds next_tag, char mode) {
    if(sdslen(next_tag) != 0) {
        sdstrim(next_tag, " ");

        switch(mode) {
            case TS_CTX_ADD_TAG: {
                char * key = malloc(strlen(next_tag));
                strcpy(key, next_tag);

                hash_set(tags, key, key); 
                break;
            }

            case TS_CTX_DEL_TAG: {
                char * val = hash_get(tags, next_tag);
                if(val != NULL) {
                    hash_del(tags, next_tag); 
                    free(val);
                }

                break;
            }
        }
        sdsfree(next_tag);
        next_tag = sdsempty();
    }

    return next_tag;
}

int tag_set(hash_t * tags, sds tag_str) {

    int tag_str_len = strlen(tag_str);

    char mode = TS_CTX_ADD_TAG;
    sds next_tag = sdsempty();

    for(int i = 0; i < tag_str_len; i++) {
        char next = tag_str[i];

        if(isspace(next)) {
            // add current item if item has any value
            if(strlen(next_tag) != 0) {
                next_tag = sdscat(next_tag, " ");
            }
        }

        else if(next == TS_CTX_DEL_TAG) {
            // if the previous character is '-' (just check at i-1), remove all items from hash
            if(i > 0 && tag_str[i-1] == TS_CTX_DEL_TAG) {
                hash_clear(tags);
                mode = TS_CTX_ADD_TAG;
            }

            else {
                next_tag = _push_tag(tags, next_tag, mode);
                mode = TS_CTX_DEL_TAG;
            }

        }

        else if(next == TS_CTX_ADD_TAG) {
            // add current item and set to '+' for next item
            next_tag = _push_tag(tags, next_tag, mode);
            mode = TS_CTX_ADD_TAG;
        }

        else {
            // append next character to next_tag
            next_tag = sdscatlen(next_tag, &next, 1);
        }
    }

    next_tag = _push_tag(tags, next_tag, mode);
    return TS_SUCCESS;
}

sds concat_string(sds str, int argc, char * argv[]) {

    for(int i = 0; i < argc; i++) {
        str = sdscat(str, argv[i]);
    }

    return str;
}

int tag_set_free(hash_t * tags) {
    hash_each(tags, {
        free(val);
    })

    hash_free(tags);
    return TS_SUCCESS;
}

int to_lower(char * self) {
    int len = strlen(self);
    for(int i = 0; i < strlen(self); i++) {
        self[i] = tolower(self[i]);
    }
    return TS_SUCCESS;
}

bool is_vowel(char x) {
    if(x == 'a' || x == 'e' || x == 'i' || x == 'o' || x == 'u') return true;
    return false;
}

sds without_vowels(sds without,char * src) {
    int len = strlen(src);
    for(int i = 0; i < len; i++) {
        if(!is_vowel(src[i])) {
            without = sdscatlen(without, &src[i], 1);
        }
    }

    return without;
}

bool begins_with(char * src, char * prefix) {
    for(int i = 0; i < strlen(prefix); i++) {
        if(prefix[i] != src[i]) {
            return false;
        }
    }
    return true;
}

bool matches(char * arg_name, char * arg_input_value) {

    bool success = false;

    char * op_lowercase = malloc(strlen(arg_input_value) + 1);
    strcpy(op_lowercase, arg_input_value);
    to_lower(op_lowercase);

    sds no_vowels = without_vowels(sdsempty(), arg_name);

    if(begins_with(arg_name, op_lowercase)) success = true;
    if(begins_with(no_vowels, op_lowercase)) success = true;

    free(op_lowercase);
    sdsfree(no_vowels);

    return success;
}

int args_create(arg_list * self, int size) {
    self->size = size;
    self->next = 0;
    self->pending_value = false;
    self->args = malloc(sizeof(arg) * size);
    return TS_SUCCESS;
}

void ** _args_add(arg_list * self, char * name, arg_type type) {

    self->args[self->next].type = type;
    self->args[self->next].name = name;
    void ** out = &self->args[self->next].value;

    self->next++;
    return out;
}

const bool _arg_true_const = true;
const bool _arg_false_const = false;
bool ** args_add_bool(arg_list * self, char * name) {
    bool ** out = (bool**)_args_add(self, name, ARG_TYPE_BOOL);
    *out = &_arg_false_const;
    return out;
}

char ** args_add_str(arg_list * self, char * name) {
    return (char**)_args_add(self, name, ARG_TYPE_STR);
}

bool _arg_set_param(arg_list * self, char * arg_input_value) {
    bool success = false;

    for(int j = 0; j < self->size; j++) {
        if(matches(self->args[j].name, arg_input_value)) {
            if(self->args[j].type == ARG_TYPE_BOOL) {
                self->args[j].value = (void*)(&_arg_true_const);
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

int args_parse(arg_list * self, int argc, char * argv[]) {

    self->pending_value = false;

    int i = 0;
    for(; i < argc; i++) {
        
        if(begins_with(argv[i], "--")) {
            sds next = sdsnew(argv[i]);
            sdstrim(next, "-");
            bool success = _arg_set_param(self, next);
            sdsfree(next);

            if(!success) {
                // error parsing argument
                break;
            }
        }

        else if(begins_with(argv[i], "-")) {

            // arguments can be packed, so each char is a seperate arg
            int argv_len = strlen(argv[i]);
            for(int j = 1; j < argv_len; j++) {
                char next_char[2];
                next_char[0] = argv[i][j];
                next_char[1] = 0;

                if(!_arg_set_param(self, next_char)) {
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

    // return what the last parsed item was. The rest of the items can be packed into a path or similar
    return i;
}

int args_close(arg_list * self) {
    free(self->args);
    return TS_SUCCESS;
}
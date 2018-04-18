#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "tsstr.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"


sds ts_str_concat_string(sds str, int argc, char * argv[]) {

    for(int i = 0; i < argc; i++) {
        str = sdscat(str, argv[i]);
    }

    return str;
}


int ts_str_to_lower(char * self) {
    int len = strlen(self);
    for(int i = 0; i < strlen(self); i++) {
        self[i] = tolower(self[i]);
    }
    return TS_SUCCESS;
}

bool ts_str_is_vowel(char x) {
    if(x == 'a' || x == 'e' || x == 'i' || x == 'o' || x == 'u') return true;
    return false;
}

sds ts_str_without_vowels(sds without,char * src) {
    int len = strlen(src);
    for(int i = 0; i < len; i++) {
        if(!ts_str_is_vowel(src[i])) {
            without = sdscatlen(without, &src[i], 1);
        }
    }

    return without;
}

bool ts_str_begins_with(char * src, char * prefix) {
    for(int i = 0; i < strlen(prefix); i++) {
        if(prefix[i] != src[i]) {
            return false;
        }
    }
    return true;
}

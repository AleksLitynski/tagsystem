#pragma once

// includes
#include "hash.h"
#include "tsstr.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

// macros

// types

// functions
int ts_str_to_lower(char * self);
bool ts_str_is_vowel(char x);
sds ts_str_without_vowels(sds without, char * src);
sds ts_str_without_spaces(sds without, char * src);
bool ts_str_begins_with(char * src, char * prefix);
sds ts_str_concat_string(sds str, int argc, char * argv[]);

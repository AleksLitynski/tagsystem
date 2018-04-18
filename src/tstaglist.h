#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros
#define TS_TAGLIST_ADD_TAG '+'
#define TS_TAGLIST_DEL_TAG '~'

// types
typedef struct ts_cli_tag {
    char operation;
    char * name;
    struct ts_cli_tag * next;
} ts_taglist;

// functions
ts_taglist * _ts_taglist_next(sds next_tag, char mode);

ts_taglist * ts_taglist_create(sds tag_str);
int ts_taglist_close(ts_taglist * head);

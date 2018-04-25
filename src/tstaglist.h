#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros
#define TS_TAGLIST_ADD_TAG '+'
// '-' is used to set cli flags, 
// so ~ is used to indicate an item should be removed from the tagset
#define TS_TAGLIST_DEL_TAG '~'

// types
typedef struct ts_cli_tag {
    // a linked list of tags and either an 'add' or 'remove' operation
    char operation;
    char * name;
    struct ts_cli_tag * next;
} ts_taglist;

// functions
ts_taglist * _ts_taglist_next(sds * next_tag, char mode);

ts_taglist * ts_taglist_create(sds tag_str);
int ts_taglist_close(ts_taglist * head);

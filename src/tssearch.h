#pragma once

// includes
#include "sds.h"
#include "tsid.h"
#include "tstags.h"

// macros

// types
typedef struct ts_search_tag {
    ts_tags * tags;
    ts_tag_node * current;
    struct ts_search_tag * parent;
    int depth;
} ts_search_tag;

typedef struct {
    int index;
    int branch;
    ts_search_tag * tags;
    int tag_count;
} ts_search;


// functions
int ts_search_create(ts_search * self, ts_db * db, ts_tags * tags, int tag_count);
int ts_search_step(ts_search * self, ts_id * id); // no item, found item, or done TS_SEARCH_DONE, TS_SEARCH_FOUND, TS_SEARCH_NONE
int ts_search_next(ts_search * self, ts_id * id); // same as ts_search_step, but automatically skops 'TS_SEARCH_NONE' results
int _ts_search_tag_close(ts_search_tag * tags);
int ts_search_close(ts_search * self);

ts_search_tag * _ts_search_item_push(ts_search_tag * tags, int branch);
ts_search_tag * _ts_search_item_pop(ts_search_tag * tags, int index);
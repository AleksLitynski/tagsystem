#pragma once

// includes
#include "sds.h"
#include "tsid.h"
#include "tstags.h"
#include "tswalk.h"

// macros

// types
typedef struct {
    int index;
    int current_id[TS_ID_BITS];
    int walk_count;
    ts_walk * walks;
} ts_search;


// functions
int ts_search_create(ts_search * self, ts_tags * tags, int tag_count);
int ts_search_step(ts_search * self, ts_id * id); // no item, found item, or done TS_SEARCH_DONE, TS_SEARCH_FOUND, TS_SEARCH_NONE
int ts_search_next(ts_search * self, ts_id * id); // same as ts_search_step, but automatically skops 'TS_SEARCH_NONE' results
int ts_search_close(ts_search * self);

int _ts_search_push(ts_search * self, int branch);
int _ts_search_pop(ts_search * self);
bool _ts_search_test(ts_search * self, int branch);
#pragma once

// includes
#include "tstags.h"

// macros

// types
typedef struct ts_walk_history_item {
    ts_tag_node * current;
    struct ts_walk_history_item * parent;
} ts_walk_history_item;

typedef struct {
    ts_tags * source;
    int index;
    int history_depth;
    ts_walk_history_item * history;
} ts_walk;


// functions
int ts_walk_create(ts_walk * self, ts_tags * tags);
int ts_walk_push(ts_walk * self, int branch);
int ts_walk_pop(ts_walk * self);
int ts_walk_close(ts_walk * self);
bool ts_walk_test(ts_walk * self, int branch);

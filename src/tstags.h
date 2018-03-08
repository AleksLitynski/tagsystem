#pragma once

// includes
#include "sds.h"
#include "tsid.h"

// macros
#define TS_TAGS_MAXRESIZE 8

// types
typedef struct {
    ts_id id;
} ts_tag_leaf;

// these are not pointers because this can be flattened to disk.
// they are the relative offset from ts_tagtree * root.
typedef struct {
    size_t _0;
    size_t _1;
} ts_tag_inner;

typedef struct {
    bool leaf;
    union {
        ts_tag_leaf leaf;
        ts_tag_inner inner;
    } value;
} ts_tag_node;

typedef struct {
    size_t size;
    size_t occupied;
    size_t next;
    ts_tag_node * data;
} ts_tags;


// functions
int ts_tagtree_empty(ts_tags * self);
int ts_tagtree_insert(ts_tags * self, ts_id * id);
int ts_tagtree_remove(ts_tags * self, ts_id * id);

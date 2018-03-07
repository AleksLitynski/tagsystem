#pragma once

// includes
#include "sds.h"
#include "tsid.h"

// macros
#define TS_TAGS_MAXRESIZE 8

// types
typedef struct {
    ts_id id;
} ts_tagleaf;

// these are not pointers because this can be flattened to disk.
// they are the relative offset from ts_tagtree * root.
typedef struct {
    uint64_t left;
    uint64_t right;
} ts_taginner;

typedef struct {
    bool leaf;
    union {
        ts_tagleaf leaf;
        ts_taginner inner;
    } value;
} ts_tagnode;

typedef struct {
    uint64_t size;
    uint64_t occupied;
    ts_tagnode * data;
} ts_tags;


// functions
int ts_tagtree_empty(ts_tags * self);
int ts_tagtree_insert(ts_tags * self, ts_id * id);
int ts_tagtree_remove(ts_tags * self, ts_id * id);

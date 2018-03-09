#pragma once

// includes
#include "sds.h"
#include "tsid.h"

// macros

// types
enum ts_tag_node_type {
    TS_TAG_NODE_LEAF,
    TS_TAG_NODE_INNER,
    TS_TAG_NODE_JUMP
};

typedef struct {
    enum ts_tag_node_type type;
    union {
        ts_id leaf;
        // these are not pointers because this can be flattened to disk.
        // they are the relative offset from ts_tagtree * root.
        size_t inner[2];
        size_t jump;
    } value;
} ts_tag_node;

typedef struct {
    size_t size;
    size_t occupied;
    size_t next;
    ts_tag_node * data;
} ts_tags;


// functions
int ts_tags_empty(ts_tags * self);
int ts_tags_empty_sized(ts_tags * self, int size);
size_t ts_tags_insert_node(ts_tags * self, ts_tag_node * to_insert);
int ts_tags_insert(ts_tags * self, ts_id * id);
int ts_tags_remove(ts_tags * self, ts_id * id);
int ts_tags_copy(ts_tags * self, ts_tag_node * source);
int ts_tags_resize(ts_tags ** self, int delta);
int ts_tags_close(ts_tags * self);
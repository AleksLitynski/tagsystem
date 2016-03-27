#ifndef _TS_TAG_H_
#define _TS_TAG_H_

// includes
#include <stdint.h>
#include "lib/lmdb/libraries/liblmdb/lmdb.h"
#include "tsenv.h"

// macros

// types
typedef struct {
    char * name;
} ts_tag;
typedef struct {
    int rootId;
    int nextId;
} ts_tag_metadata;
typedef struct {
    unsigned int key; // 0 means allocate a new one for me
    uint8_t * doc_id_fragment;
    uint8_t * mask;
    unsigned int * jumps;
} ts_tag_node;

// functions
void ts_tag_create(ts_env * env, char * tagName, ts_tag * tag);
void ts_tag_close(ts_env * env, ts_tag * tag);
void ts_tag_insert(ts_env * env, ts_tag * tag, ts_doc_id * doc);
void ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_tag_node * node);

// tsdoc uses these, otherwise, they can be private.
void _ts_mdb_val_to_tag_node(MDB_val * val, int id_size_bits, ts_tag_node * node);
void _ts_tag_node_to_mdb_val(
        ts_tag_node * node, 
        int id_size_bits, int starting_ofset_bits, 
        unsigned int new_jump, int new_jump_index,
        MDB_val * val);
 
#endif

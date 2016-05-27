#ifndef _TS_NODE_H_
#define _TS_NODE_H_

// includes
#include <stdint.h>
#include "tsutil.h"
#include "lmdb.h"

// macros

// types
typedef struct {
    int size;
    unsigned int key; // 0 means allocate a new one for me
    uint8_t * doc_id;
    uint8_t * mask;
    unsigned int * jumps;
} ts_node;
typedef uint8_t ts_node_cmp[TS_NODE_BYTES];

// functions
/*
void ts_node_compress(ts_node * node, ts_node_cmp * cmp);
void ts_node_inflate(ts_node_cmp * cmp, int size, ts_node * node);

void ts_node_create(ts_node * node);
void ts_node_close(ts_node * node);
void ts_node_copy(ts_node * from, ts_node * to);

void ts_node_cmp_create(ts_node_cmp * cmp);
void ts_node_cmp_close(ts_node_cmp * cmp);
void ts_node_cmp_copy(ts_node_cmp * from, ts_node * to);
*/

// tsdoc uses these, otherwise, they can be private.
void ts_node_from_mdb_val(MDB_val * val, int id_size_bits, ts_node * node);
void ts_node_to_mdb_val(
        ts_node * node, 
        int id_size_bits, int starting_offset_bits, 
        unsigned int new_jump, int new_jump_index,
        MDB_val * val);
 
#endif

#ifndef _TS_TAG_H_
#define _TS_TAG_H_

// includes
#include <stdint.h>
#include "lib/lmdb/libraries/liblmdb/lmdb.h"
#include "tsenv.h"
#include "tsnode.h"

// macros

// types
typedef struct {
    char * name;
} ts_tag;

// functions
void ts_tag_create(ts_env * env, char * tagName, ts_tag * tag);
void ts_tag_close(ts_env * env, ts_tag * tag);
void ts_tag_insert(ts_env * env, ts_tag * tag, ts_doc_id * doc);
void ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_node * node);

#endif

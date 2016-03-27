#ifndef _TS_SEARCH_H_
#define _TS_SEARCH_H_

#include "tswalk.h"
#include "tsdoc.h"
#include "tsenv.h"
#include "lib/lmdb/libraries/liblmdb/lmdb.h"

typedef struct {
    int index,
    int tagCount,
    ts_walk * nodes,
    ts_doc_id * next
} ts_search;
int  ts_search_reset(ts_search * search);
int  ts_search_pop(ts_search * search);
int  ts_search_push(ts_search * search, int branch);
int  ts_search_next(ts_env * env, ts_search * search);
void ts_search_close(ts_env * env, ts_search * search);
void ts_search_create(ts_env * env, MDB_val * tags, ts_doc_id * first, ts_search * search);


#endif

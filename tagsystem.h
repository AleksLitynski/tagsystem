#ifdef _TS_H_
#define _TS_H_

#include <stdint.h>
include "lib/lmdb/libraries/liblmdb/lmdb.h"

typedef char[20] ts_doc_id;
typedef struct { 
    int size; char * data[]; 
} ts_tags;
typedef struct { 
    MDB_env * env; char * docs; char * index; char * iIndex; 
} ts_env;
typedef struct {
    // probably need to pack these 3 values by hand
    unsigned int pathSize: 8; // max 160, needs 8 bits
    unsigned int maskMask: 20; // (this should often be less than 20, as the path will often be less than 20) allignment of masks relative to path (size: pathLength/bits-in-a-char)
    unsigned int jumpSize: 8; // total number of jumps (max 160)

    char * path; // 1 - 160 bits of data
    char * mask; // series of 1 char masks
    void * jumps; // the jumps indicated by the mask

    // [ pl ][ mm ][ gc ][ path ][ jumps ]
    //  a, b, c, d | e, f, g, h | i, j, k, l // pathLength: 12 
    //  0, 0, 1, 0 |            | 0, 0, 0, 1 // maskMask: 101
    //  ptr, ptr, 0, 0, 0, 0, 0, 0, 0, 0 
    //      // on delete: remove item. if more than 10 spaces, de-allocate
    //      // on add: add to end. If no space, compact all
    //      //      if still no space, allocate 
}


// api
void ts_doc_create(ts_env * env, MDB_val * content, ts_doc_id * id) {
void ts_doc_get(ts_env * env, ts_doc_id * doc, MDB_val * doc);
void ts_doc_del(ts_env * env, ts_doc_id * doc);  
void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tag); 
void ts_doc_untag(ts_env * env, ts_doc_id * doc, char * tag);

// set maxDocs = 0 for all docs
void ts_search_begin(ts_env * env, ts_tags * tags, ts_doc_id first, ts_search * search);
int  ts_search_next(ts_search * search, ts_tags * tags, MDB_val * next); 
void ts_search_end(ts_search * search);

ts_env    ts_env_open(MDB_env * menv, char * prefix[]);
ts_env    ts_env_open_full(char filename[], char *prefix[]);
void      ts_env_close(ts_env * env);
void      ts_env_close_full(ts_env * env);


// utilities

// create node
// grow node
// shrink node
// get valid path from index
//
// init workspace
//      
#endif

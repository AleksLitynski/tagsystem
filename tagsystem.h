#ifdef _TS_H_
#define _TS_H_

#include <stdint.h>
include "lib/lmdb/libraries/liblmdb/lmdb.h"

// key size macros
#define TS_KEY_SIZE_BITS 160
#define TS_KEY_SIZE_BYTES 20
#define TS_KEY_SIZE_BITS_DOUBLE 320
#define TS_KEY_SIZE_BYTES_DOUBLE 40
#define TS_MAX_NODE_SIZE_BITS (TS_KEY_SIZE_BITS_DOUBLE + (sizeof(unsigned int) * TS_KEY_SIZE_BITS))
#define TS_MAX_NODE_SIZE_BYTES (TS_KEY_SIZE_BYTES_DOUBLE + (sizeof(unsigned int) * TS_KEY_SIZE_BYTES))

typedef uint8_t[TS_KEY_SIZE_BYTES] ts_doc_id;
typedef struct { 
    int size; char * data[]; 
} ts_tags;
typedef struct { 
    MDB_env * env; 
    char * docs; // could replace with a dbi somehow?
    char * index; 
    char * iIndex; 
} ts_env;
typedef struct {
    char * name;
} ts_tag;
typedef struct {
    int rootId;
    int nextId;
    
    // gaps starts at 10, jumps has 0 items
    // on insert, count out to the last spot. 
    //      - If the last item is 0, add the item to the end
    //      - Otherwise compact and add
    //      - Otherwise re-allocate
    // on delete: set item to 0, incriment gaps
    //      - if gaps is >= 20, de-allocate
} ts_tag_metadata;
typedef struct {
    unsigned int key; // 0 means allocate a new one for me
    uint8_t * doc_id_fragment;
    uint8_t * mask;
    unsigned int * jumps;
} ts_tag_node;

// api
void ts_doc_create(ts_env * env, MDB_val * content, ts_doc_id * id);
void ts_doc_get(ts_env * env, ts_doc_id * doc, MDB_val * doc);
void ts_doc_del(ts_env * env, ts_doc_id * doc);
void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tag);
void ts_doc_untag(ts_env * env, ts_doc_id * doc, char * tag);

void ts_tag_create(ts-env * env, char * tagName, ts_tag * tag);
void ts_tag_close(ts_env * env, ts_tag * tag);
void ts_tag_insert(ts_env * env, ts_tag * tag, ts_doc_id * doc);

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

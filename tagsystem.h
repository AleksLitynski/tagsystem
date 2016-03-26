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

// api
typedef uint8_t[TS_KEY_SIZE_BYTES] ts_doc_id;
void ts_doc_create(ts_env * env, MDB_val * content, ts_doc_id * id);
void ts_doc_get(ts_env * env, ts_doc_id * doc, MDB_val * doc);
void ts_doc_del(ts_env * env, ts_doc_id * doc);
void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tag);
void ts_doc_untag(ts_env * env, ts_doc_id * doc, char * tag);

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
void ts_tag_create(ts-env * env, char * tagName, ts_tag * tag);
void ts_tag_close(ts_env * env, ts_tag * tag);
void ts_tag_insert(ts_env * env, ts_tag * tag, ts_doc_id * doc);
void ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_tag_node * node);

typedef struct { 
    MDB_env * env; 
    char * docs; // could replace with a dbi somehow?
    char * index; 
    char * iIndex; 
} ts_env;
ts_env ts_env_open(MDB_env * menv, char * prefix[]);
ts_env ts_env_open_full(char filename[], char *prefix[]);
void   ts_env_close(ts_env * env);
void   ts_env_close_full(ts_env * env);

typedef struct {
    char * tag,
    int index,
    ts_tag_node * current,
    int offset,
    int jumps,
    int historyIndex,
    _ts_walk_history * history
} ts_walk;
typedef struct {
    unsigned int id,
    int offset,
    int jumps
} _ts_walk_history;
int  ts_walk_reset(ts_env * env, ts_walk * walk); 
int  ts_walk_push(ts_env * env, ts_tag_walk * walk, int path);
int  ts_walk_pop(ts_env * env, ts_tag_walk * walk);
void ts_walk_close(ts_env * env, ts_walk * walk);
void ts_walk_create(ts_env * env, ts_walk * walk, char * tagname);

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

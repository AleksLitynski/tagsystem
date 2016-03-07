#ifdef _TS_H_
#define _TS_H_

#include <stdint.h>
include "lib/lmdb/libraries/liblmdb/lmdb.h"

// 5, [*, *, *, *, *] count followed by count pointers
#define LIST(T) struct {unsigned int length; T data[];}
typedef struct {uint64_t a; uint64_t b; uint32_t c;} ts_doc_id;
typedef LIST(char) ts_doc; // docs can contain \0
typedef LIST(ts_doc_id *) ts_docs; // pointers to doc_ids
typedef LIST(char *) ts_tags; // pointers to char array
typedef struct {
    MDB_env * env; 
    char * docs; 
    char * index; 
    char * iIndex;
} ts_env;


// api
ts_doc_id ts_doc_create(ts_env * env, ts_doc * content);
void      ts_doc_delete(ts_env * env, ts_doc_id * doc);  
ts_doc    ts_doc_get(ts_env * env, ts_doc_id * doc);
ts_tags   ts_doc_tags(ts_env * env, ts_doc_id * doc);
void      ts_doc_tag(ts_env * env, ts_doc_id * doc, ts_tag * tag);
void      ts_doc_untag(ts_env * env, ts_doc_id * doc, ts_tag * tag);

// set maxDocs = 0 for all docs
ts_docs   ts_search(ts_env * env, ts_tags tags, ts_doc_id minDoc, int maxDocs); 
ts_docs   ts_search_all(ts_env * env, ts_tags tags); // minDoc = 0, maxDocs = 0

ts_env    ts_open(MDB_env * menv, char * prefix[]);
ts_env    ts_open_full(char filename[], char *prefix[]);
void      ts_close(ts_env * env);
void      ts_close_full(ts_env * env);


// utilities

// create node
// grow node
// shrink node
// get valid path from index
//
// init workspace
//      
#endif

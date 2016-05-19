#ifndef _TS_TAG_H_
#define _TS_TAG_H_

// includes
#include <stdint.h>
#include "lmdb.h"
#include "tsenv.h"
#include "tsnode.h"
#include "tsdoc.h"

// macros

// types
typedef struct {
    unsigned int rootId; 
    unsigned int nextId;
} ts_tag_metadata;

typedef struct {
    char ** tags;
    int count;
} ts_tags;

// functions
void ts_tag_create(ts_env * env, char * tag);
void ts_tag_insert(ts_env * env, char * tag, ts_doc_id * doc);
void ts_tag_remove(ts_env * env, char * tag, ts_doc_id * doc);

#endif

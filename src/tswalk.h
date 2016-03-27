#ifndef _TS_WALK_H_
#define _TS_WALK_H_

// includes
#include "tsenv.h"
#include "tstag.h"

// macros

// types
typedef struct {
    char * tag,
    int index,
    ts_tag_node * current,
    int offset,
    int jumps,
    int historyIndex,
    ts_walk_history * history
} ts_walk;

typedef struct {
    unsigned int id,
    int offset,
    int jumps
} ts_walk_history;

// functions
int  ts_walk_reset(ts_env * env, ts_walk * walk); 
int  ts_walk_push(ts_env * env, ts_tag_walk * walk, int path);
int  ts_walk_pop(ts_env * env, ts_tag_walk * walk);
void ts_walk_close(ts_env * env, ts_walk * walk);
void ts_walk_create(ts_env * env, ts_walk * walk, char * tagname);

#endif

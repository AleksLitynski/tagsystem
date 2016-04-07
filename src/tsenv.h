#ifndef _TS_ENV_H_
#define _TS_ENV_H_

// includes
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"

// macros

// types
typedef struct { 
    MDB_env * env; 
    char * dir; // could replace with a dbi somehow?
    // dir/
    //  + index [mdb]
    //      + index
    //      + tKEY_NAME
    //  + docs/
    //      + ab [first two char of doc id
    //          + c-aa [last 38 char of doc id]
} ts_env;

// functions
void ts_env_create(char * path, ts_env * env);
void ts_env_close(ts_env * env);

#endif

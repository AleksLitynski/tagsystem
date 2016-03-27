#ifndef _TS_ENV_H_
#define _TS_ENV_H_

// includes
#include "lib/lmdb/libraries/liblmdb/lmdb.h"

// macros

// types
typedef struct { 
    MDB_env * env; 
    char * docs; // could replace with a dbi somehow?
    char * index; 
    char * iIndex; 
} ts_env;

// functions
ts_env ts_env_create(MDB_env * menv, char * prefix[]);
ts_env ts_env_create_full(char filename[], char *prefix[]);
void   ts_env_close(ts_env * env);
void   ts_env_close_full(ts_env * env);

#endif

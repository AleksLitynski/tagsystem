#include "tsenv.h"

#include <stdlib.h>
#include "tsutil.h"


ts_env ts_env_create(MDB_env * menv, char * prefix) {
    ts_env env = malloc(sizeof(ts_env));
    env = {
        .env    = menv,
        .doc    = ts_util_concat(prefix, "_docs"),
        .index  = ts_util_concat(prefix, "_index"),
        .iIndex = ts_util_concat(prefix, "_iIndex")
    }
    return env;
}

ts_env ts_env_create_full(char * filename, char * prefix) {
    MDB_env *menv;
    mdb_env_create(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);

    ts_util_mkdir_safe(filename);
    mdb_env_open(menv, filename, mdb_fixedmap, 0664);
    return ts_open(menv, prefix);
}

void ts_env_close(ts_env * env) {
    // http://stackoverflow.com/questions/1518711/
    //      how-does-free-know-how-much-to-free
    free(env->doc);
    free(env->index);
    free(env->iIndex);
}

void ts_env_close_full(ts_env * env) {
    mdb_env_close(env->env);
    ts_close(env);
}


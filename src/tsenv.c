#include "tsenv.h"

#include <stdlib.h>
#include "tsutil.h"

void ts_env_create(char * path, ts_env * env) {
    MDB_env * menv;
    mdb_env_create(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);

    ts_util_safe_mk(path);

    char * indexDB = ts_util_concat(path, "/index");
    ts_util_safe_mk(indexDB);
    mdb_env_open(menv, indexDB, MDB_FIXEDMAP, 0664);
    free(indexDB);

    char * docDB = ts_util_concat(path, "/docs");
    ts_util_safe_mk(docDB);
    free(docDB);

    env->env = menv;
    env->dir = malloc(strlen(path));
    strcpy(env->dir, path);
}

void ts_env_close(ts_env * env) {
    mdb_env_close(env->env);
    free(env->dir);
}


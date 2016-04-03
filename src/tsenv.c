#include "tsenv.h"

#include <stdlib.h>
#include "tsutil.h"

void ts_env_create(char * path, ts_env * env) {
    MDB_env * menv;
    mdb_env_creatte(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);

    ts_util_mkdir_safe(path);

    char * indexDB = t_util_concat(path, "/index");
    ts_util_mkdir_safe(indexDB);
    mdb_env_open(menv, indexDB, mdb_fixedmap, 0664);
    free(indexDB);

    char * docDB = t_util_concat(path, "/docs");
    ts_util_mkdir_safe(docDB);
    free(docDB);

    env->env = menv;
    env->path = malloc(strlen(path));
    strcpy(env->path, path);
}

void ts_env_close(ts_env * env) {
    mdb_env_close(env->env);
    free(env->path);
}


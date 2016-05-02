#include "tsenv.h"
#include "lmdb.h"

#include <stdlib.h>
#include "tsutil.h"

#include <stdio.h>

void ts_env_create(char * path, ts_env * env) {

    env->dir = strdup(path);
    env->index = ts_util_concat(path, "/index");
    env->docs = ts_util_concat(path, "/docs");
    ts_util_safe_mk(env->dir);
    ts_util_safe_mk(env->index);
    ts_util_safe_mk(env->docs);

    mdb_env_create(&env->env);
    mdb_env_set_maxreaders(env->env, 1);
    mdb_env_set_mapsize(env->env, 10485760);
    mdb_env_set_maxdbs(env->env, 10000); // THIS IS IMPORTANT. THIS IS THE MAX # OF TAGS!! (and its bad if its too big?, accoring to lmdbs documentation? This may require re-designing :( probably to not use lmdb, which is totally the 'wrong tool for the job' at this point :(
    mdb_env_open(env->env, env->index, 0, 0664);
}

void ts_env_close(ts_env * env) {
    mdb_env_close(env->env);
    free(env->dir);
    free(env->index);
    free(env->docs);
}


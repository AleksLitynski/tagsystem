#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "tssearchset.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

ts_search * ts_searchset_create(ts_cli_ctx * ctx) {

    ts_tags_readonly * tags = malloc(sizeof(ts_tags) * hash_size(ctx->pws));

    MDB_txn * txn;
    int i = 0;
    hash_each(ctx->pws, {
        ts_tags_open_readonly(&tags[i], ctx->db, key, &txn);
        i++;
    })
    mdb_txn_commit(txn);

    ts_search * search = malloc(sizeof(search));
    ts_search_create(search, tags->tags, hash_size(ctx->pws));

    return search;
}

void ts_searchset_close(ts_search * search) {
    for(int i = 0; i < search->walk_count; i++) {
        ts_tags_close(search->walks[i].source);
    }
    free(search->walks[0].source);
    ts_search_close(search);
    free(search);
}


bool ts_searchset_has_one(ts_cli_ctx * ctx) {
    ts_search * search = ts_searchset_create(ctx);
    bool has_one = true;
    ts_id id;
    if(ts_search_next(search, &id) != TS_SEARCH_DONE
    && ts_search_next(search, &id) != TS_SEARCH_DONE) {
        has_one = false;
    }

    ts_searchset_close(search);

    return has_one;
}

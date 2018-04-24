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

ts_search * ts_searchset_create(ts_cli_ctx * ctx, hash_t * pws) {

    ts_tags * tags = malloc(sizeof(ts_tags) * hash_size(pws));
    ts_db_begin_txn(ctx->db);

    int i = 0;
    hash_each(pws, {
        // printf("%s\n", key);
        ts_tags_open(&tags[i], ctx->db, key);
        // ts_tags_log(&tags[i]);
        i++;
    })

    ts_search * search = malloc(sizeof(ts_search));
    ts_search_create(search, tags, hash_size(pws));

    return search;
}

void ts_searchset_close(ts_cli_ctx * ctx, ts_search * search) {
    for(int i = 0; i < search->walk_count; i++) {
        ts_tags_close(search->walks[i].source);
    }
    free(search->walks[0].source);
    ts_search_close(search);
    free(search);
    ts_db_commit_txn(ctx->db);
}


bool ts_searchset_has_one(ts_cli_ctx * ctx, hash_t * pws) {
    ts_search * search = ts_searchset_create(ctx, pws);
    bool has_one = true;
    ts_id id;
    if(ts_search_next(search, &id) != TS_SEARCH_DONE
    && ts_search_next(search, &id) != TS_SEARCH_DONE) {
        has_one = false;
    }

    ts_searchset_close(ctx, search);

    return has_one;
}

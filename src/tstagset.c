
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "tstaglist.h"
#include "tstagset.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

int ts_tagset_append(hash_t ** tags, sds tag_str) {

    sds full_tag_str = ts_tagset_print(*tags);
    full_tag_str = sdscat(full_tag_str, "+");
    full_tag_str = sdscat(full_tag_str, tag_str);
    ts_taglist * head = ts_taglist_create(full_tag_str);

    ts_tagset_close(*tags);
    *tags = hash_new();

    ts_taglist * current = head;

    do {

        switch(current->operation) {
            case TS_TAGLIST_ADD_TAG: {
                char * key = malloc(strlen(current->name));
                strcpy(key, current->name);

                hash_set(*tags, key, key); 
                break;
            }

            case TS_TAGLIST_DEL_TAG: {
                char * val = hash_get(*tags, current->name);
                if(val != NULL) {
                    hash_del(*tags, current->name); 
                    free(val);
                }

                break;
            }
        }

        current = current->next;

    } while(current != 0);


    sdsfree(full_tag_str);
    ts_taglist_close(head);
    return TS_SUCCESS;
}

hash_t * ts_tagset_load(ts_cli_ctx * ctx) {
    hash_t * t = hash_new();
    MDB_val val;
    ts_db_begin_txn(ctx->db);
    int res = ts_db_get(ctx->db, "meta", "TSPWS", &val);
    
    if(res == TS_SUCCESS) {
        char * pws_str = calloc(val.mv_size + 1, 1);
        strcpy(pws_str, val.mv_data);
        pws_str[val.mv_size] = '\0';
        
        if(val.mv_size != 0) {
            ts_tagset_append(&t, pws_str);
        }
        free(pws_str);

    }

    ts_db_commit_txn(ctx->db);

    return t;
}

void ts_tagset_save(ts_cli_ctx * ctx, hash_t * tags) {
    sds pws_str = ts_tagset_print(tags);
    MDB_val val = {
        .mv_data = pws_str,
        .mv_size = sdslen(pws_str)
    };

    ts_db_begin_txn(ctx->db);
    ts_db_del(ctx->db, "meta", "TSPWS", NULL);
    if(val.mv_size != 0) {
        ts_db_put(ctx->db, "meta", "TSPWS", &val);
    }

    ts_db_commit_txn(ctx->db);
    sdsfree(pws_str);
}

sds ts_tagset_print(hash_t * tags) {
    sds pws_str = sdsempty();
    hash_each(tags, {
        pws_str = sdscat(pws_str, "+");
        pws_str = sdscat(pws_str, key);
    })
    return pws_str;
}

int ts_tagset_close(hash_t * tags) {
    hash_each(tags, {
        free(val);
    })

    hash_free(tags);
    return TS_SUCCESS;
}
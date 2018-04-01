#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "main.h"
#include "parsing.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

ts_db * get_db() {
    char * db_path = getenv("TSDBPATH");
    if(db_path == 0) db_path = "~/.tsysdb";
    ts_db * db = malloc(sizeof(ts_db));
    ts_db_open(db, db_path);
    return db;
}

hash_t * get_working_set() {
    hash_t * working_set = hash_new();
    char * current_tags_str = getenv("TSPWS");
    if(current_tags_str != 0) tag_set(working_set, current_tags_str);
    return working_set;

}

ts_search * search_set(cli_ctx * ctx) {

    ts_tags_readonly * tags = malloc(sizeof(ts_tags) * hash_size(ctx->pws));

    MDB_txn * txn;
    int i = 0;
    hash_each(ctx->pws, {
        ts_tags_open_readonly(&tags[i], db, key, &txn);
        i++;
    })
    mdb_txn_commit(txn);

    ts_search * search = malloc(sizeof(search));
    ts_search_create(search, tags->tags, hash_size(pws));

    return search;
}

void search_set_close(ts_search * search) {
    for(int i = 0; i < search->walk_count; i++) {
        ts_tags_close(search->walks[i].source);
    }
    free(search->walks[0].source);
    ts_search_close(search);
    free(search);
}

void print_id(cli_ctx * ctx, ts_id * id, bool show_id) {
    // print the id or doc
    if(show_id) {
        sds id_str = ts_id_string(id, sdsempty());
        printf("%s\n", &id_str);
        sdsfree(id_str);
    } else {
        // make a doc, print the path
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, *id);
        printf("%s\n", doc.path);
        ts_doc_close(&doc);
    }
}

void save_pws(cli_ctx * ctx) {
    sds pws_str = print_pws(ctx);
    setenv("TSPWS", pws_str, true);
    sdsfree(pws_str);
}

sds print_pws(cli_ctx *  ctx) {
    sds pws_str = sdsempty();
    hash_each(ctx->pws, {
        pws_str = sdscat(pws_str, "+");
        pws_str = sdscat(pws_str, key);
    })
    return pws_str;
}

bool set_has_one_item(cli_ctx * ctx) {
    ts_search * search = search_set(ctx);
    bool has_one = true;
    ts_id id;
    if(ts_search_next(search, &id) != TS_SEARCH_DONE
    && ts_search_next(search, &id) != TS_SEARCH_DONE) {
        has_one = false;
    }

    search_set_close(search);

    return has_one;
}

bool confirm(char * message) {
    char input[100] = {0};
    while(!matches("yes", input) && !matches("no", input)) {
        printf("%s [y/n]\n", message);
        fgets(input, 100, stdin);   
    }

    return matches("yes", input);
}


sds * stdin_to_array(int * count) {
    sds input = sdsempty();

    char * next[1000] = {0};
    while(fgets(next, 1000, stdin)) {
        input = sdscat(input, next);
    }

    return sdssplitlen(input, sdslen(input), "\n", 1, count);
}

char * doc_path_id(sds doc_path) {
    char * doc_path_dup = strdup(doc_path);
    char * doc_name_temp = dirname(doc_path_dup);
    char * doc_name = strdup(doc_name_temp);
    free(doc_path_dup);
    free(doc_name_temp);
    return doc_name;
}
#include "ts.h"
#include <paths.h>
#include "khash.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tsiter.h"
#include "errno.h"
#include "lmdb.h"

#define TS_OP_ADD 0
#define TS_OP_REM 1
KHASH_SET_INIT_STR(str)


void ts_cws(char * path) {
    ts_env env;
    ts_env_create(path, &env);
    ts_env_close(&env);

    char * fullPath = realpath(path, NULL);
    setenv("TSPATH", fullPath, 1);
    free(fullPath);
}

const char * ts_pws() {
    char * out = getenv("TSPWS");
    if(out) {
        return out;
    } else {
        return "";
    }
}


void _ts_cs_insert(int op, khash_t(str) * h, char * item, int * itemLen) {
    if(*itemLen > 0) {
        item[*itemLen] = '\0';
        if(op == TS_OP_ADD) {
            int absent;
            khint_t k = kh_put(str, h, item, &absent);
            if (absent) {
                kh_key(h, k) = strdup(item);
            }
        } else if(op == TS_OP_REM) {
            khint_t k = kh_get(str, h, item);
            free(kh_key(h, k)); // free the data
            kh_del(str, h, k); // remove from table
        }
    }

    *itemLen = 0;
}

void _ts_cs_clear(khash_t(str) * h) {
for (khint_t k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            char * item = kh_key(h, k);
            free(item);
            kh_del(str, h, k); 
        }
    }
}

// -- to clear
// +  to add    (default)
// -  to remove
void ts_cs(char * set) {
    khash_t(str) * h = kh_init(str);
    char * pset = ts_pws();
    int mlen = strlen(pset) + 1 + strlen(set) + 1;
    char * mset = malloc(mlen);
    strcpy(mset, pset);
    mset[strlen(pset)] = '+';
    strcpy(&mset[strlen(pset)+1], set);
    mset[mlen] = '\0';

    int setLen = strlen(mset);
    char * item = malloc(setLen);
    int itemLen = 0;
    int op = TS_OP_ADD;
    for(int i = 0; i < setLen; i++) {
        if(isspace(mset[i])) {
            _ts_cs_insert(op, h, item, &itemLen);
        } else if(op == TS_OP_REM && itemLen == 0 && mset[i] == '-') {
            _ts_cs_clear(h);
            op = TS_OP_ADD;
        } else if(mset[i] == '+') {
            _ts_cs_insert(op, h, item, &itemLen);
            op = TS_OP_ADD;
        } else if(mset[i] == '-') {
            _ts_cs_insert(op, h, item, &itemLen);
            op = TS_OP_REM;
        } else {
            item[itemLen] = mset[i];
            itemLen++;
        }
    }
    _ts_cs_insert(op, h, item, &itemLen);
    free(item);
    free(mset);

    char * oset = malloc(mlen); // first + and last \0
    int olen = 0;
    for (khint_t k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            char * item = kh_key(h, k);
     
            if(olen != 0) {
                oset[olen] = '+';
                olen++;
            }

            strcpy(&(oset[olen]), item);
            olen+= strlen(item);
            free(item);
        }
    }
    oset[olen] = '\0';
    setenv("TSPWS", oset, 1);
    free(oset);

    kh_destroy(str, h);
}


bool _ts_is_empty(char * str) {
    while(*str != '\n') {
        if(!isspace(*str)) return false;
    }
    return true;
}


void ts_mk0(){ ts_doc_id id; ts_mk(&id); } 
void ts_mk(ts_doc_id * id) {

    ts_env env;
    ts_env_create(getenv("TSPATH"), &env);
    ts_doc_create(&env, id);
    // tag the doc with it's id
    char * id_str = ts_util_str_id(id);
    ts_tag_insert(&env, id_str, id);
    free(id_str);

    // tag the doc will all tags in the pws
    char * tspws = strdup(getenv("TSPWS"));
    char * head = tspws;
    for(int i = 0; i < strlen(tspws); i++) {
        if(tspws[i] == '+') {
            tspws[i] = '\0';
            if(!_ts_is_empty(head)) ts_tag_insert(&env, head, id);
            head = &(tspws[i+1]);
        }
    }
    if(!_ts_is_empty(head)) ts_tag_insert(&env, head, id); 
    free(tspws);

    ts_env_close(&env);
}

void ts_rm() {
    iter0(ts_ls, id, {
        ts_doc_del(id_ctx.env, id);
    })
}


void ts_ls_init(ts_ls_ctx * ctx, ts_ls_item * item) {

    ctx->env = malloc(sizeof(ts_env));
    ts_env_create(getenv("TSPATH"), ctx->env);

    // count the tags in the pws
    ctx->tags = malloc(sizeof(ts_tags));
    ctx->set = strdup(getenv("TSPWS"));
    ctx->tags->count = 1;
    int len = strlen(ctx->set);
    for(int i = 0; i < len; i++) {
        if(ctx->set[i] == '+') {
            ctx->set[i] = '\0';
            ctx->tags->count++;
        }
    }

    // make space for the tags in the ctx
    ctx->tags->tags = malloc(sizeof(char *) * ctx->tags->count);
    // add the tags to the ctx
    int index = 0;
    int lastItem = 0;
    for(int i = 0; i < len; i++) {
        if(ctx->set[i] == '\0' || i == len - 1) { 
            char * nextWord = ctx->set + lastItem;
            ctx->tags->tags[index] = nextWord;
            lastItem = i + 1;
            index++;
        }
    }
     
    // zero out the counter
    ctx->first = calloc(TS_ID_BYTES, sizeof(uint8_t));
    ctx->search = malloc(sizeof(ts_search));
    ts_search_create(ctx->env, ctx->tags, ctx->first, ctx->search);

    // zero out the 'return' value
    *item = malloc(sizeof(ts_doc_id));
    for(int i = 0; i < TS_ID_BYTES; i++) {
        (**item)[i] = 0;
    }
}

void ts_ls_close(ts_ls_ctx * ctx, ts_ls_item * item) {
    ts_search_close(ctx->env, ctx->search);
    ts_env_close(ctx->env);
    free(ctx->first);
    free(ctx->tags->tags);
    free(ctx->set);
    free(ctx->env);
    free(ctx->tags);
    free(ctx->search);
    free(*item);
}
int ts_ls_next(ts_ls_ctx * ctx, ts_ls_item * item) {
    // next search result, return res
    int res = ts_search_next(ctx->env, ctx->search);
    if(res) {
        *item = ctx->search->next;
        return 1;
    } else {
        return 0;
    }
}

void ts_tag(char * tag) {
    printf("the reads begin...\n");


    ts_ls_ctx id_ctx;
    ts_ls_item id;
    ts_ls_init(&id_ctx, &id);

    id_ctx.search->index = 0;
    while(ts_ls_next(&id_ctx, &id)) {

        printf("ts_ls_next returned once \n");
        ts_tag_insert(id_ctx.env, tag, id);
    };
    ts_ls_close(&id_ctx, &id);



/*
    iter0(ts_ls, id, {
        printf("the reads begin...\n");
        ts_tag_insert(id_ctx.env, tag, id);
    })
    */
}

void ts_untag(char * tag) {
    iter0(ts_ls, id, {
        ts_tag_remove(id_ctx.env, tag, id);
    })
} 

char * ts_resolve(ts_doc_id * id) {
    char * dir = getenv("TSPATH");
    char * item = malloc(strlen(dir) +1 +40 +1);
 
    char folder[3];
    folder[0] = (char)*id[0]; 
    folder[1] = (char)*id[1];
    folder[2] = '\0';

    char file[39];
    int fpos = 0;
    for(int i = 2; i < TS_ID_BYTES;  i++) {
        file[fpos] = (char)*id[i];
        fpos++;
    }
    file[38] = '\0';

   sprintf(item, "%s/%s/%s", dir, folder, file);
   return item;
} 

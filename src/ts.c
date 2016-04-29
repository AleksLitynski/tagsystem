#include "ts.h"
#include "khash.h"
#include <stdio.h>
#include <ctype.h>
#include "tsiter.h"
#include "errno.h"

#define TS_OP_ADD 0
#define TS_OP_REM 1
KHASH_SET_INIT_STR(str)


void ts_cws(char * path) {
    // char * fullPath = realpath(path, NULL);
    ts_env * env;
    ts_env_create(path, env);
    ts_env_close(env);

    // free(fullPath);
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

// -- to clear
// +  to add    (default)
// -  to remove
void ts_cs(char * set) {
    khash_t(str) * h = kh_init(str);

    int setLen = strlen(set);
    char * item = malloc(setLen);
    int itemLen = 0;
    int op = TS_OP_ADD;
    int addExisting = 1;


    for(int i = 0; i < setLen; i++) {
        // if whitespace, try to add next
        // if set to -, nlen == 0, char == -, addExisting = false
        // if operator, set the operator
        // otherwise, append to next
        
        if(isspace(set[i])) {
            _ts_cs_insert(op, h, item, &itemLen);
        } else if(op == TS_OP_REM && itemLen == 0 && set[i] == '-') {
            _ts_cs_insert(op, h, item, &itemLen);
            addExisting = 0;
            op = TS_OP_ADD;
        } else if(set[i] == '+') {
            _ts_cs_insert(op, h, item, &itemLen);
            op = TS_OP_ADD;
        } else if(set[i] == '-') {
            _ts_cs_insert(op, h, item, &itemLen);
            op = TS_OP_REM;
        } else {
            item[itemLen] = set[i];
            itemLen++;
        }

    }
    _ts_cs_insert(op, h, item, &itemLen);
    free(item);

    // if -- wasn't in the set, 
    //  re-add the existing set
    char * pset = ts_pws();

    if(addExisting) {
        // inflate TSPWS into tree
        char * pItem = malloc(strlen(pset));
        int pItemLen = 0;
        for(int i = 0; i < strlen(pset); i++) { if(!isspace(set[i])) {
            if(set[i] == '+') {
                _ts_cs_insert(TS_OP_ADD, h, pItem, &pItemLen);
            } else {
                pItem[pItemLen] = pset[i];
                pItemLen++;
            }
        }}
        _ts_cs_insert(TS_OP_ADD, h, pItem, &pItemLen);
        free(pItem);
    }

    char * oset = malloc(strlen(set) + strlen(pset) + 2); // first + and last \0
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
    kh_destroy(str, h);

    oset[olen] = '\0';
    setenv("TSPWS", oset, 1);
    free(oset);

}



void ts_mk0(){ ts_doc_id * id; ts_mk(id); } 
void ts_mk(ts_doc_id * id) {

    ts_env * env;
    ts_env_create(getenv("TSPATH"), env);
    ts_doc_create(env, id);
    // tag the doc with it's id
    ts_tag_insert(env, (char *)id, id);

    // tag the doc will all tags in the pws
    char * tspws = strdup(getenv("TSPWS"));
    char * head = tspws;
    for(int i = 0; i < strlen(tspws); i++) {
        if(tspws[i] == '+') {
            tspws[i] = '\0';
            ts_tag_insert(env, (char *)id, id);
            head = &(tspws[i+1]);
        }
    }

    free(tspws);
    ts_env_close(env);
}

void ts_rm() {
    iter0(ts_ls, id, {
        ts_doc_del(id_ctx.env, id);
    })
}


void ts_ls_init(ts_ls_ctx * ctx, ts_ls_item * item) {

    ts_env_create(getenv("TSPATH"), ctx->env);
    ts_tags * tags; 

    // count the tags in the pws
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
    ctx->tags->tags = malloc(sizeof(char) * ctx->tags->count);
    ctx->tags->tags = ctx->set;
    // add the tags to the ctx
    int inum = 1;
    for(int i = 0; i < len; i++) {
        if(i < len-1) { //if we're not on the last one
            ctx->tags->tags[inum] = ctx->set[i+1];
            inum++;
        }
    }
     
    // zero out the counter
    ctx->first = calloc(TS_KEY_SIZE_BYTES, sizeof(uint8_t));
    ts_search_create(ctx->env, ctx->tags, ctx->first, ctx->search);

    // zero out the 'return' value
    for(int i = 0; i < TS_KEY_SIZE_BYTES; i++) {
        item[i] = 0;
    }
}

void ts_ls_close(ts_ls_ctx * ctx, ts_ls_item * item) {
    ts_search_close(ctx->env, ctx->search);
    ts_env_close(ctx->env);
    free(ctx->first);
    free(ctx->tags->tags);
    free(ctx->set);
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
    iter0(ts_ls, id, {
        ts_tag_insert(id_ctx.env, tag, id);
    })
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
    for(int i = 2; i < TS_KEY_SIZE_BYTES;  i++) {
        file[fpos] = (char)*id[i];
        fpos++;
    }
    file[38] = '\0';

   sprintf(item, "%s/%s/%s", dir, folder, file);
   return item;
} 

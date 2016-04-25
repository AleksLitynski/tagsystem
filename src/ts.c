#include "ts.h"
#include "kbtree.h"
#include <stdio.h>
#include <ctype.h>
#include "tsiter.h"

#define TS_OP_ADD 0
#define TS_OP_REM 1
typedef struct {
    char * key;
} elem_t;
#define elem_cmp(a, b) (strcmp((a).key, (b).key))
KBTREE_INIT(str, elem_t, elem_cmp)


void ts_cws(char * path) {
    char * fullPath = realpath(path, NULL);
    setenv("TSPATH", fullPath, 1); //1 = replace if exists
    ts_env * env;
    ts_env_create(fullPath, env);
    ts_env_close(env);
    free(fullPath);
}

const char * ts_pws() {
    return getenv("TSPWS");
}


// -- to clear
// +  to add    (default)
// -  to remove
void ts_cs(char * set) {
    kbtree_t(str) * sTree = kb_init(str, KB_DEFAULT_SIZE);

    // add new data to tree 
    elem_t next = {.key = malloc(strlen(set)) }; // the max size possible
    int nLen = 0;
    int op = TS_OP_ADD;
    int addExisting = 1;

    for(int i = 0; i < strlen(set); i++) {
        // if whitespace, try to add next
        // if set to -, nlen == 0, char == -, addExisting = false
        // if operator, set the operator
        // otherwise, append to next

        if(isspace(set[i])) {
            if(nLen > 0) {
                next.key[nLen] = '\0';
                if(op == TS_OP_ADD) {
                    elem_t * entry = kb_getp(str, sTree, &next);
                    if(!entry) kb_putp(str, sTree, &next);
                } else if(op == TS_OP_REM) {
                    kb_del(str, sTree, next);
                }
            }
            nLen = 0;
        } else if(op == TS_OP_REM && nLen == 0 && set[i] == '-') {
            addExisting = 0;
            op = TS_OP_ADD;
            nLen = 0;
        } else if(op == '+') {
            op = TS_OP_ADD;
            nLen = 0;
        } else if(op == '-') {
            op = TS_OP_REM;
            nLen = 0;
        } else {
            next.key[nLen] = set[i];
            nLen++;
        }

    }
    free(next.key);

    // if -- wasn't in the set, 
    //  re-add the existing set
    char * pset = ts_pws();
    if(addExisting) {
        // inflate TSPWS into tree
        elem_t pnext = {.key = malloc(strlen(pset))};
        int plen = 0;
        for(int i = 0; i < strlen(pset); i++) { if(!isspace(set[i])) {
            if(set[i] == '+') {
                pnext.key[plen] = '\0';
                elem_t * entry = kb_getp(str, sTree, &pnext);
                if(!entry) kb_putp(str, sTree, &pnext);
                plen = 0;
            } else {
                pnext.key[plen] = set[i];
                plen++;
            }
        }}
        free(pnext.key);
    }

    // flatten the tree into a string
    char * oset = malloc(strlen(set) + strlen(pset) + 2); // first + and last \0
    int olen = 0;
    kbitr_t itr;
    kb_itr_first(str, sTree, &itr);
    for(; kb_itr_valid(&itr); kb_itr_next(str, sTree, &itr)) {
        elem_t itm = kb_itr_key(elem_t, &itr);
        
        // don't preface the first char with a +
        if(olen != 0) {
            oset[olen] = '+';
            olen++;
        }

        strcpy(&(oset[olen]), itm.key);
        olen+= strlen(itm.key);

    }
    kb_destroy(str, sTree);
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

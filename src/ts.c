#include "ts.h"
#include "kbtree.h"
#include <stdio.h>
#include <ctype.h>

#define TS_OP_ADD 0
#define TS_OP_REM 1
typedef struct {
    char * key;
} elem_t;
#define elem_cmp(a, b) (strcmp((a).key, (b).key))
KBTREE_INIT(str, elem_t, elem_cmp)


void ts_cws(char * path) {
    setenv("TSPATH", path, 1); //1 = replace if exists
    ts_env * env;
    ts_env_create(path, env);
    ts_env_close(env);
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



char * ts_mk(char * set) {

    ts_env * env;
    ts_doc_id * id;
    ts_env_create(getenv("TSPATH"), env);
    ts_doc_create(env, id);
    char * idstr = malloc(sizeof(char) * 41);
    for(int i = 0; i < 40; i++) {
        idstr[i] = ts_util_test_bit((uint8_t *)id, i);
    }
    idstr[40] = '\0';

    // tag the doc with it's id
    ts_doc_tag(env, id, idstr);


    // tag the doc will all tags in the pws
    ts_cs(set);
    char * tspws = strdup(getenv("TSPWS"));
    char * head = tspws;
    for(int i = 0; i < strlen(tspws); i++) {
        if(tspws[i] == '+') {
            tspws[i] = '\0';
            ts_doc_tag(env, id, head);
            head = &(tspws[i+1]);
        }
    }

    free(tspws);
    ts_env_close(env);
    return idstr;
}

void ts_rm(char * set) {
    ts_cs(set);
    
    ts_ls_ctx * ctx;
    ts_ls_item * item;
    ts_ls_init("", ctx, item);
    while(ts_search_next(ctx->env, ctx->search)) {
        // delete all docs in the given set
        ts_doc_delete(ctx->env, ctx->search->next);
    }
    ts_ls_close(ctx, item);
}


void ts_ls_init(char * set, ts_ls_ctx * ctx, ts_ls_item * item) {

    ts_cs(set);

    ts_env_create(getenv("TSPATH"), ctx->env);
    ts_tags * tags; 

    ctx->set = strdup(getenv("TSPWS"));
    ctx->tags->count = 1;
    int len = strlen(ctx->set);
    for(int i = 0; i < len; i++) {
        if(ctx->set[i] == '+') {
            ctx->set[i] = '\0';
            ctx->tags->count++;
        }
    }

    ctx->tags->tags = malloc(sizeof(char) * ctx->tags->count);
    ctx->tags->tags = ctx->set;
    int inum = 1;
    for(int i = 0; i < len; i++) {
        if(i < len-1) { //if we're not on the last one
            ctx->tags->tags[inum] = ctx->set[i+1];
            inum++;
        }
    }
     
    ctx->first = calloc(TS_KEY_SIZE_BYTES, sizeof(uint8_t));
    ts_search_create(ctx->env, ctx->tags, ctx->first, ctx->search);

    //                  /path/to/file       / ab / c-zz
    *item = malloc(strlen(getenv("TSPATH") + 1 + 40 + 1));
}
void ts_ls_close(ts_ls_ctx * ctx, ts_ls_item * item) {
    ts_search_close(ctx->env, ctx->search);
    ts_env_close(ctx->env);
    free(ctx->first);
    free(ctx->tags->tags);
    free(ctx->set);
    free(item);
}
int ts_ls_next(ts_ls_ctx * ctx, ts_ls_item * item) {
    int res = ts_search_next(ctx->env, ctx->search);
    if(res) {

        char dir[3];
        dir[0] = ts_util_test_bit((uint8_t *)ctx->search->next, 0);
        dir[1] = ts_util_test_bit((uint8_t *)ctx->search->next, 1);
        dir[2] = '\0';

        char file[39];
        for(int i = 2; i < 40; i++) {
            file[i] = ts_util_test_bit((uint8_t *)ctx->search->next, i);
        }
        file[38] = '\0';

        sprintf(*item, "%s/%s/%s", getenv("TSPATH"), dir, file);

        return 1;
    } else {
        return 0;
    }
}

void ts_tag(char * tag, char * set) {
    ts_cs(set);

    ts_ls_ctx * ctx;
    ts_ls_item * item;
    ts_ls_init("", ctx, item);
    while(ts_search_next(ctx->env, ctx->search)) {
        ts_doc_tag(ctx->env, ctx->search->next, tag);
    }
    ts_ls_close(ctx, item);
}

void ts_untag(char * tag, char * set) {
    ts_cs(set);

    ts_ls_ctx * ctx;
    ts_ls_item * item;
    ts_ls_init("", ctx, item);
    while(ts_search_next(ctx->env, ctx->search)) {
        ts_doc_tag(ctx->env, ctx->search->next, tag);
        ts_doc_untag(ctx->env, ctx->search->next, tag);
    }
    ts_ls_close(ctx, item);
}

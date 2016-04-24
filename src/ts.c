#include "ts.h"
#include "lib/klib/kbtree.h"

#define CS_OP_ADD 0
#define TS_OP_REM 1
#define CS_OP_REP 2
KBTREE_INIT(str, char *, strcmp)


void ts_cws(char * path) {
    setenv("TSPATH", path, 1); //1 = replace if exists
}

char * ts_pws() {
    return getenv("TSPWS");
}


// -- to clear
// +  to add    (default)
// -  to remove
void ts_cs(char * set) {
    kbtree_t(str) * sTree = kb_init(str, KB_DEFAULT_SIZE);

        // add new data to tree 
    char * next = malloc(strlen(set)); // the max size possible
    int nLen = 0;
    int op = CS_OP_ADD;
    int addExisting = 1;

    for(int i = 0; i < strlen(set); i++) {
        // if whitespace, try to add next
        // if set to -, nlen == 0, char == -, addExisting = false
        // if operator, set the operator
        // otherwise, append to next

        if(isspace(set[i])) {
            if(nLen > 0) {
                op[nLen] = '\0';
                if(op == CS_OP_ADD) {
                    char * entry = kb_getp(str, sTree, next);
                    if(!entry) kb_putp(str, sTree, next);
                } else if(op == CS_OP_REM) {
                    kh_del(str, sTree, next);
                }
            }
            nLen = 0;
        } else if(op == CS_OP_REM && nLen == 0 && set[i] == '-') {
            addExisting = 0;
            op = CS_OP_ADD;
            nLen = 0;
        } else if(op == '+') {
            op = CS_OP_ADD;
            nLen = 0;
        } else if(op == '-') {
            op = CS_OP_REM;
            nLen = 0;
        } else {
            next[nLen] = set[i];
            nLen++;
        }

    }
    free(next);

    char * pset = ts_pws();
    if(addExisting) {
        // inflate TSPWS into tree
        char * pnext = malloc(strlen(pset));
        int plen = 0;
        for(int i = 0; i < strlen(pset); i++) { if(!isspace(set[i])) {
            if(set[i] == '+') {
                pnext[plen] = '\0';
                char * entry = kb_getp(str, sTree, &pnext);
                if(!entry) kb_putp(str, sTree, &pnext);
                plen = 0;
            } else {
                pnext[plen] = set[i];
                plen++;
            }
        }}
        free(pnext);
    }

    // flatten the tree into a string
    char * oset = malloc(strlen(set) + strlen(pset) + 2); // first + and last \0
    int olen = 0;
    kbiter_t itr;
    kb_itr_first(str, sTree, &itr);
    for(; kb_itr_valid(&itr); kb_itr_next(str, sTree, &itr)) {
       char * itm = &kb_itr_key(char *, &itr);
        
       // don't preface the first char with a +
       if(olen != 0) {
            oset[olen] = '+';
            olen++;
       }

       strcpy(&(oset[olen]), itm);
       olen+= strlen(itm);

    }
    kb_destroy(str, sTree);
    oset[olen] = '\0';

    setenv("TSPWS", oset, 1);
    free(oset);
    kbdestroy(sTree);
}



char * ts_mk() {
    ts_env * env;
    ts_doc_id * id;
    ts_env_create(getenv("TSPATH"), env);
    ts_doc_create(env, id);
    char idstr[41];
    for(int i = 0; i < 40; i++) {
        idstr[i] = ts_util_test_bit(id, i);
    }
    idstr[40] = '\0';
    ts_doc_tag(env, id, idstr);
    ts_env_close(env);
}


typedef struct {
    ts_env * env,
    ts_search * search,
    ts_doc_id * first,
    char * set
} ts_ls_ctx;  
typedef char * ts_ls_item;

void ts_ls_init(ts_ls_ctx * ctx, ts_ls_item * item) {
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

    ctx->tags->tags = malloc(sizeof(char) * items);
    ctx->tags->tags[0] = ctx->set;
    int item = 1;
    for(int i = 0; i < len; i++) {
        if(i < len-1) { //if we're not on the last one
            ctx->tags->tags[inum] = &(ctx->set[i+1]);
            item++;
        }
    }
     
    ctx->first = calloc(TS_KEY_SIZE_BYTES);
    ts_search_create(ctx->env, ctx->tags, ctx->first, ctx->search);

    //                  /path/to/file       / ab / c-zz
    item = malloc(strlen(getenv("TSPATH") + 1 + 40 + 1);
}
void ts_ls_close(ts_ls_ctx * ctx, ts_ls_item * item) {
    ts_search_close(ctx->search);
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
        data[0] = ts_util_test_bit(ctx->search->next, 0);
        data[1] = ts_util_test_bit(ctx->search->next, 1);
        data[2] = '\0';

        char file[39];
        for(int i = 2; i < 40; i++) {
            data[i] = ts_util_test_bit(ctx->search->next, i);
        }
        file[38] = '\0';

        sprintf(item, "%s/%s/%s", getenv("TSPATH"), dir, file);

        return 1;
    } else {
        return 0;
    }
}

void ts_tag(char * tag) {
    ts_ls_ctx * ctx;
    ts_ls_item * item;
    ts_ls_init(ctx, item);
    while(ts_search_next(ctx->env, ctx->search) {
        ts_doc_tag(ctx->env, ctx->search->next, tag);
    }
    ts_ls_close(ctx, item);
}

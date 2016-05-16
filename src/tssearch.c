#include "tssearch.h"

#include <stdlib.h>
#include <stdio.h>


// void ts_walk_create(ts_env * env, ts_walk * walk, char * tagname) {
void ts_search_create(ts_env * env, ts_tags * tags, ts_doc_id * first, ts_search * search) {
    search->index = 0;
    search->tagCount = tags->count;
    search->next = malloc(TS_KEY_SIZE_BYTES);
    search->nodes = malloc(sizeof(ts_walk) * tags->count);
    for(int i = 0; i < tags->count; i++) {
        ts_walk_create(env, &search->nodes[i], &((char *)tags->tags)[i]);
    }
}

void ts_search_close(ts_env * env, ts_search * search) {
    free(search->next);
    for(int i = 0; i < search->tagCount; i++) {
        ts_walk_close(env, &search->nodes[i]);
    }
    free(search->nodes);
}

int _ts_search_push(ts_env * env, ts_search * search, int branch);
int _ts_search_reset(ts_env * env, ts_search * search);
int _ts_search_pop(ts_env * env, ts_search * search);
int  ts_search_next(ts_env * env, ts_search * search) {
    // walk the tree to the next value 

    printf("first result requested <---\n");
    
    _ts_search_reset(env, search);
    ts_doc_id next;

    while(search->index < TS_KEY_SIZE_BITS) {
        
        if(_ts_search_push(env, search, 0)) {
            next[search->index/8] &= ~(1<<(search->index%8));
        }

        if(_ts_search_push(env, search, 1)) {
            next[search->index/8] |= 1<<(search->index%8);
        }

        if(!_ts_search_pop(env, search)) {
            printf("woo first real bug (it shouldnt exit with no result\n");
            return 0;
        }
    }

    for(int i = 0; i < TS_KEY_SIZE_BITS; i++) {
        *search->next[i/8] = next[i/8];
    }



    return 1;
}

int _ts_search_push(ts_env * env, ts_search * search, int branch) {
    if(branch < ts_util_test_bit(*search->next, search->index)) return 0;

    // push each node
    int i = 0;
    for(;i < search->tagCount; i++) {
        if(~ts_walk_push(env, &search->nodes[i], branch)) {
            break; 
        }
    }

    if(i < search->tagCount) {
        // unwind 
        for(;i > 0; i--) {
            ts_walk_pop(env, &search->nodes[i]);
        }
        return 0;
    } else {
        return 1;
    }
}

int _ts_search_pop(ts_env * env, ts_search * search) {
    search->index--;
    for(int i = 0; i < search->tagCount; i++) {
        ts_walk_pop(env, &search->nodes[i]);
    }
    return 0;
}

int _ts_search_reset(ts_env * env, ts_search * search) {
    search->index = 0; 
    for(int i = 0; i < search->tagCount; i++) {
        ts_walk_reset(env, &search->nodes[i]);
    }
    return 0;
}



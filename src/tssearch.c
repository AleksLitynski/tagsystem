#include "tssearch.h"

#include <stdlib.h>


void ts_search_create(ts_env * env, MDB_val * tags, ts_doc_id * first, ts_search * search) {
    search->index = 0;
    search->tagCount = tags->mv_size;
    search->next = malloc(TS_KEY_SIZE_BYTES);
    for(int i = 0; i < tags->mv_size; i++) {
        ts_walk_create(env, search->nodes[i], tags->mv_data[i]);
    }
}

void ts_search_close(ts_env * env, ts_search * search) {
    free(search->next);
    for(int i = 0; i < search->tagCount; i++) {
        ts_walk_close(env, search->nodes[i]);
    }
}

int  ts_search_next(ts_env * env, ts_search * search) {
    // walk the tree to the next value 
    
    ts_search_reset(search);
    ts_doc_id next;

    while(search->index < TS_KEY_SIZE_BITS) {
        
        if(ts_search_push(search, 0)) {
            next[search->index/8] &= ~(1<<(search->index%8));
        }

        if(ts_search_push(search, 1)) {
            next[search->index/8] |= 1<<(search->index%8);
        }

        if(!ts_search_pop(search)) {
            return 0;
        }
    }

    for(int i = 0; i < TS_KEY_SIZE_BITS; i++) {
        search->next[i/8] = next[i/8];
    }

    return 1;
}

int ts_search_push(ts_search * search, int branch) {
    if(branch < ts_util_test_bit(search->next, search->index)) return 0;

    // push each node
    int i = 0;
    for(i < search->tagCount; i++) {
        if(~ts_walk_push(search->nodes[i], branch)) {
            break; 
        }
    }

    if(i < search->tagCount) {
        // unwind 
        for(i > 0; i--) {
            ts_walk_pop(search->nodes[i]);
        }
    } else {
        return 1;
    }
}

int ts_search_pop(ts_search * search) {
    search->index--;
    for(int i = 0; i < search->tagCount; i++) {
        ts_walk_pop(search->nodes[i]);
    }
}

int ts_search_reset(ts_search * search) {
    search->index = 0; 
     for(int i = 0; i < search->tagCount; i++) {
        ts_wak_reset(search->nodes[i]);
    }
}



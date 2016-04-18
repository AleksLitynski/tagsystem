#include "tswalk.h"

#include <stdlib.h>
#include "tsutil.h"
#include "lmdb.h"


void ts_walk_create(ts_env * env, ts_walk * walk, char * tagname) {
    walk->tag = tagname;
    walk->index = 0;    
    walk->offset = 0;
    walk->jumps = 0;
    walk->historyIndex = -1;
    walk->history = malloc(TS_KEY_SIZE_BITS * sizeof(ts_walk_history));

    walk->current->key = 0;
    walk->current->doc_id = malloc(TS_KEY_SIZE_BYTES);
    walk->current->mask = malloc(TS_KEY_SIZE_BYTES);
    walk->current->jumps = malloc(sizeof(unsigned int) * TS_KEY_SIZE_BITS);
}

void ts_walk_close(ts_env * env, ts_walk * walk) {
    free(walk->history);
    free(walk->current->doc_id);
    free(walk->current->mask);
    free(walk->current->jumps);
}

int _ts_walk_copy_to_node(ts_env * env, ts_walk * walk, unsigned int key);
int ts_walk_pop(ts_env * env, ts_walk * walk) {
    walk->index--;
    walk->offset = walk->history[walk->historyIndex].offset;
    walk->jumps = walk->history[walk->historyIndex].jumps;
    _ts_walk_copy_to_node(env, walk, walk->history[walk->historyIndex].id);
    walk->historyIndex--;

    return 0;
}

int ts_walk_push(ts_env * env, ts_walk * walk, int path) {

    int hasJump = ts_util_test_bit(walk->current->mask, walk->index - walk->offset + 1);
    if(ts_util_test_bit(walk->current->doc_id, walk->index - walk->offset + 1)) {
        walk->index++;
        if(hasJump) walk->jumps++;
        return 1;
        
    } else if(hasJump) {
        walk->historyIndex++;
        walk->history[walk->historyIndex].id = walk->current->key;
        walk->history[walk->historyIndex].offset = walk->offset;
        walk->history[walk->historyIndex].jumps = walk->jumps;
 
        walk->jumps = 0;
        walk->offset += walk->index;
        walk->index++;
       _ts_walk_copy_to_node(env, walk, walk->current->jumps[walk->jumps]);
    }

    return 0;
    // is the next item in current?
    //      advance the index, inc jump (if needed)
    // is the next item in the jump?
    //      push current to history 
    //      put the jump into current
    //
    // else, return false, don't advance or anything

}

int ts_walk_reset(ts_env * env, ts_walk * walk) {
    walk->index = 0;
    walk->offset = 0;
    walk->jumps = 0;
    walk->historyIndex = -1;

    _ts_walk_copy_to_node(env, walk, 0);

    return 0;
}

// take the node of the iIndex and copy it the the walks' current node
int _ts_walk_copy_to_node(ts_env * env, ts_walk * walk, unsigned int host_key) {

    // open txn
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, * value;

    char * tagName = ts_util_concat("t", walk->tag);
    key->mv_size = sizeof(unsigned int);
    key->mv_data = key; 

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tagName, MDB_INTEGERKEY, dbi);
    mdb_get(txn, *dbi, key, value);

    walk->current->key = (unsigned int)key->mv_data;

    int jumps = 0;
    for(int i = 0; i < TS_KEY_SIZE_BITS - walk->offset; i++) {
        if(ts_util_test_bit(value->mv_data, i)) {
            walk->current->doc_id[i/8] |= 1<<(i%8);
        }
        if(ts_util_test_bit(&((uint8_t *)value->mv_data)[TS_KEY_SIZE_BITS - walk->offset], i)) {
            walk->current->mask[i/8] |= 1<<(i%8);
            walk->current->jumps[jumps] = ((unsigned int *)&value->mv_data)[
                ((TS_KEY_SIZE_BITS - walk->offset) * 2) + 
                (sizeof(unsigned int) * jumps)];

            jumps++;
        }
    }

    mdb_txn_commit(txn);
    free(tagName);

    return 0;
}

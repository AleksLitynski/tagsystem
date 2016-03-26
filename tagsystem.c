#include "tagsystem.h"
#include "util.h"


ts_env ts_open(MDB_env * menv, char * prefix) {
    ts_env env = malloc(sizeof(ts_env));
    env = {
        .env    = menv,
        .doc    = ts_util_concat(prefix, "_docs"),
        .index  = ts_util_concat(prefix, "_index"),
        .iIndex = ts_util_concat(prefix, "_iIndex")
    }
    return env
}

ts_env ts_open_full(char * filename, char * prefix) {
    MDB_env *menv;
    mdb_env_create(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);

    ts_util_mkdir_safe(filename);
    mdb_env_open(menv, filename, mdb_fixedmap, 0664);
    return ts_open(menv, prefix);
}

void ts_close(ts_env * env) {
    // http://stackoverflow.com/questions/1518711/
    //      how-does-free-know-how-much-to-free
    free(env->doc);
    free(env->index);
    free(env->iIndex);
}

void ts_close_full(ts_env * env) {
    mdb_env_close(env->env);
    ts_close(env);
}


void ts_doc_create(ts_env * env, MDB_val * content, ts_doc_id * id) {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key;

    ts_util_gen_doc_id(content, id);
    key.mv_size = sizeof(ts_doc_id);
    key.mv_data = id;

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->doc, MDB_CREATE, &dbi);

    mdb_put(txn, dbi, &key, &content, 0);
    mdb_txn_commit(txn);
}	

void ts_doc_get(ts_env * env, ts_doc_id * id, MDB_val * doc) {
    MDB_txn * txn;
    MDB_dbi dbi;
    MDB_val key;

    key.mv_size = sizeof(ts_doc_id);
    key.mv_data = id;

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->doc, MDB_CREATE, &dbi);
    
    mdb_get(txn, dbi, key, doc);

    mdb_txn_commit(txn);
    
}

void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tagName) {
    // get the tag-tree
    // insert the doc-id into the tree
    // close the handle on the tree
    ts_tag * tag;
    ts_tag_create(env, tagName, tag); 
    ts_tag_insert(env, tag, doc);
    ts_tag_close(env, tag);
}

void ts_doc_untag(ts_env * env, ts_doc_id * doc, char * tag) {
    // grab the tag tree
    // travsere to the node that ends with this document
    // migrate all the branches to the parent, except this node
    // (can't migrate to parent, need to... something)
    // remove this node
    


    /*
    1111111
    0001010
       
          1
          0

        111
        100

         11
         01

     it's not obvious that without the stalk the branches will
     be asssignable to a single root. We DO know that the branches
     of the branches will still be assigned to the same parent (I think?)
     */

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, meta, dbOut;
    ts_tag_node current;
    MDB_val data[2];
    data[0].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    data[1].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);

    // setup transaction
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag->name, MDB_INTEGERKEY, &dbi);
    key.mv_size = sizeof(unsigned int); 

    // get metadata
    key.mv_data = 0;
    mdb_get(txn, dbi, meta_key, meta);

    // get root node
    key->mv_data = meta->mv_data.rootId;
    int res = mdb_get(txn, dbi, key, dbOut);
    
    if(res == MDB_NOTFOUND) {
        // these is no root - our item has already been removed
        mdb_txn_commit(txn);
        return;             
    } else {

        // there is a root. Walk the tree to find our leaf
        current.key = key->mv_data;
        _ts_mdb_val_to_tag_node(dbOut, TS_KEY_SIZE_BITS, current);

        // the depth of the current node
        int nodeInset = 0; 
        int maskCount = 0;
        unsigned int prevId = current.key;
        int prevMaskCount = 0;

        int bitIndex = 0;
        for(bitIndex < TS_KEY_SIZE_BITS; bitIndex++) {
            int localIndex = bitIndex - nodeInset;
            uint8_t bitHasMask = ts_util_test_bit(current->mask, localIndex);
             
            if(ts_util_test_bit(node->doc_id, bitIndex) == ts_util_test_bit(current->doc_id, localIndex)) {
                if(bitHasMask) maskCount++;
                //  this branch has it, continue
                continue; 
            } else if(bitHasMask) {
                prevId = current.key;
                prevMaskCount = maskCount;

                // the other branch has it, jump, then continue
                key->mv_data = current->jumps[maskCount];
                mdb_get(txn, dbi, key, dbOut);  // overwrite current node
                current.key = key->mv_data;
                _ts_mdb_val_to_tag_node(dbOut, TS_KEY_SIZE_BITS - bitIndex, current);

                nodeInset = bitIndex;           // increase the inset
                maskCount = 0;                  // reset the mask count
                continue;
            } else {
                // we coudn't find this document in the tree
                mdb_txn_commit(&txn);
                return;
            }
        }

        if(bitIndex >= TS_KEY_SIZE_BITS) {
            // remove from parent
            MDB_val * dbOutParent;
            key->mv_data = prevId;
            mdb_get(txn, dbi, key, dbOutParent); 
            // create a mask such that the jump will be turned off
            parentData->mv_data[prevMaskCount/8] &= ~(1<<(prevMaskCount%8));
            mdb_put(txn, dbi, key, dbOutParent);

            // re-insert all children
            int jumpNum = 0;
            for(int i = 0; i < TS_KEY_SIZE_BITS - node_inset; i++) {
                if(ts_util_test_bit(current->mask, i)) {
                                    
                    // I'm not sure if this will overwrite the value of dbOut before
                    //      the txn completes, of if it's ok.
                    key->mv_data = current->jumps[jumpNum];
                    mdb_get(txn, dbi, key, dbOut);
                    ts_tag_move(txn, &data, env, key, dbOut);

                    jumpNum++;
                }
            }

            // remove from lmdb
            key->mv_data = current->key;
            mdb_delete(txn, dbi, key, dbOut);
        
        }

        mdb_txn_commit(txn);
        free(data[0]->mv_data);
        free(data[1]->mv_data);
}

void ts_tag_create(ts_env * env, char * tagName, ts_tag * tag) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * meta_key, meta_data, root_keytag;

    tag->name = ts_util_concat(env->iIndex, tag);

    // create the meta (0) item, if it doesn't exist
    meta_key.mv_size = sizeof(unsigned int); 
    meta_key.mv_data = 0;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag->name, MDB_CREATE | MDB_INTEGERKEY, &dbi);
    int res = mdb_get(txn, dbi, meta_key, meta_data);
    if(res == MDB_NOTFOUND) {
        meta_data->rootId = 1;
        meta_data->nextId = 2;
        mdb_put(txn, dbi, meta_key, tagTree);
    }

    mdb_txn_commit(txn);
}

void ts_tag_close(ts_env * env, ts_tag * tag) {
    free(tag->name);
}

void ts_tag_insert(ts_env * env, ts_tag * tag, ts_doc_id * doc) {
    uint8_t mask[TS_KEY_SIZE_BYTES] = {0};
    ts_tag_node node;
    MDB_txn * txn;
    MDB_val data[2];

    data[0].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    data[1].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    mdb_txn_begin(txn, &data, env->env, NULL, 0, &txn);
    node.key = 0;
    node.doc_id = doc;
    mode.mask = &mask;
    ts_tag_move(env, tag, &node);
    mdb_txn_commit(&txn);
    free(data[0]->mv_data);
    free(data[1]->mv_data);

}

void ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_tag_node * node) {
    // variables
    MDB_dbi * dbi;
    MDB_val * key, meta, dbOut;
    ts_tag_node current;

    // setup transaction
    mdb_dbi_open(txn, tag->name, MDB_INTEGERKEY, &dbi);
    key.mv_size = sizeof(unsigned int); 

    // get metadata
    key.mv_data = 0;
    mdb_get(txn, dbi, meta_key, meta);

    // get root node
    key->mv_data = meta->mv_data.rootId;
    int res = mdb_get(txn, dbi, key, dbOut);
    
    if(res == MDB_NOTFOUND) {
        node->key = key->mv_data;

        _ts_tag_node_to_mdb_val(
            node, 
            TS_KEY_SIZE_BITS, 0, 
            0, 0, 
            &new_data[0]);

        mdb_put(txn, dbi, key, new_data[0]);
        free(new_data[0]->mv_data);
        return;             
 
    } else {

        // there is a root. Walk the tree and insert ourself once the walk runs out
        current.key = key->mv_data;
        _ts_mdb_val_to_tag_node(dbOut, TS_KEY_SIZE_BITS, current);

        // the depth of the current node
        int nodeInset = 0; 
        int maskCount = 0;

        for(int bitIndex = 0; bitIndex < TS_KEY_SIZE_BITS; bitIndex++) {
            int localIndex = bitIndex - nodeInset;
            uint8_t bitHasMask = ts_util_test_bit(current->mask, localIndex);
             
            if(ts_util_test_bit(node->doc_id, bitIndex) == ts_util_test_bit(current->doc_id, localIndex)) {
                if(bitHasMask) maskCount++;
                //  this branch has it, continue
                continue; 
            } else if(bitHasMask) {
                // the other branch has it, jump, then continue
                key->mv_data = current->jumps[maskCount];
                mdb_get(txn, dbi, key, dbOut);  // overwrite current node
                current.key = key->mv_data;
                _ts_mdb_val_to_tag_node(dbOut, TS_KEY_SIZE_BITS - bitIndex, current);

                nodeInset = bitIndex;           // increase the inset
                maskCount = 0;                  // reset the mask count
                continue;
            } else {
                //  neither branch exists. Insert the data

                // add the new node
                // copy the mask/jumps the user provided
                _ts_tag_node_to_mdb_val(
                    node, 
                    TS_KEY_SIZE_BITS - bitIndex, bitIndex, 
                    0, 0, 
                    &new_data[0]);
                if(node->key == 0) {
                    // if the user provided a key, use it
                    node->key = meta->mv_data.nextId;
                    meta->mv_data.nextId++;
                }
                key->mv_data = node->key;
                mdb_put(txn, dbi, key, &new_data[0]);

                // update parent mask
                int parentTotalSize = current->mv_size + sizeof(unsigned int);
                int parentDataSize = TS_KEY_SIZE_BITS - nodeInset;

                _ts_tag_node_to_mdb_val(
                    current, 
                    TS_KEY_SIZE_BITS - nodeInset, nodeInset, 
                    newNode->key, bitIndex - nodeInset, 
                    &new_data[1]);

                key->mv_data = current->key;
                mdb_put(txn, dbi, key, new_data[1]);
                return;             
            }
        }
    }
}

// val->mv_data must have TS_MAX_NODE_SIZE_BYTES free space 
void _ts_tag_node_to_mdb_val(
        ts_tag_node * node, 
        int id_size_bits, int starting_ofset_bits, 
        unsigned int new_jump, int new_jump_index,
        MDB_val * val) {
    
    int idSizeBytes = (id_size_bits+8-1)/8;
    int jumpCount = 0;
    for(int i = 0; i < idSizeBytes; i++) { 
        val->mv_data[i] = 0; 
    }
    for(int i = 0; i < id_size_bits; i++){
        int j = i + starting_offset_bits;
        val->mv_data[i/8] |= node->doc_id[j/8]&(1<<j%8);

        uint8_t mask = node->mask[j/8]&(1<<j%8);
        if(new_jump && new_jump_index == i) mask = (1<<j%8);
        val->mv_data[id_size_bits + (i/8)] |= mask;

        if(mask) {
            unsigned int * jump = (unsigned int)(&val->mv_data[idSizeBytes * 2]);
            jump += jumpCount * sizeof(unsigned int);
            if(new_jump && new_jump_index == i) {
                *jump = new_jump;
            } else {
                *jump = node->jumps[jumpCount];
            }
            jumpCount++;
        }
    }

    val->mv_size = (idSizeBytes * 2) + (jumpCount * sizeof(unsigned int));
}

// node will point into val, so don't let val be de-allocated
void _ts_mdb_val_to_tag_node(MDB_val * val, int id_size_bits, ts_tag_node * node) {
    int idSizeBytes = (id_size_bits+8-1)/8;
    uint8_t * data = val->mv_data; 
    node->doc_id = data; 
    node->mask = &data[idSizeBytes];
    node->jumps = (unsigned int)(&data[idSizeBytes * 2]);
}

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

void ts_walk_create(ts_env * env, ts_walk * walk, char * tagname) {
    walk->tag = tagname;
    walk->index = 0;    
    walk->offset = 0;
    walk->jumps = 0;
    walk->historyIndex = -1;
    walk->history = malloc(TS_KEY_SIZE_BITS * sizeof(_ts_walk_history));
    walk->current = {
        .key = 0,
        .doc_id_fragment = malloc(TS_KEY_SIZE_BYTES),
        .mask = malloc(TS_KEY_SIZE_BYTES),
        .jumps = malloc(sizeof(unsigned int) * TS_KEY_SIZE_BITS)
    };
}

void ts_walk_close(ts_env * env, ts_walk * walk) {
    free(walk->history);
    free(walk->current->doc_id_fragment);
    free(walk->current->mask);
    free(walk->current->jumps);
}

int ts_walk_pop(ts_env * env, ts_tag_walk * walk) {
    walk->index--;
    walk->offset = history[historyIndex]->offset;
    walk->jumps = history[historyIndex]->jumps;
    _ts_walk_copy_to_node(env, walk, history[historyIndex]->id);
    historyIndex--;
}

int ts_walk_push(ts_env * env, ts_tag_walk * walk, int path) {
    int hasLeft, hasRight;

    int hasJump = ts_util_test_bit(walk->current->mask, walk->index - walk->offset + 1);
    if(ts_util_test_bit(walk->current->doc_id_fragment, walk->index - walk->offset + 1)) {
        walk->index++;
        if(hasJump) walk->jumps++;
        return 1;
        
    } else if(hasJump) {
        walk->historyIndex++;
        history[walk->historyIndex] = {
            .id = walk->current->key,
            .offset = walk->offset,
            .jumps = walk->jumps,
        };
 
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
}

int _ts_walk_copy_to_node(ts_env * env, ts_walk * walk, unsigned int key) {

    // open txn
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, value;

    char * tagName = ts_util_concat(env->iIndex, walk->tag);
    key->mv_size = sizeof(unsigned int);
    key->mv_data = key; 

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tagName, MDB_INTEGERKEY, &dbi);
    int res = mdb_get(txn, dbi, key, value);

    walk->current->key = key->mv_data;

    int jumps = 0;
    for(int i = 0; i < TS_KEY_SIZE_BITS - walk->offset; i++) {
        if(ts_util_test_bit(value->mv_data, i)) {
            walk->current->doc_id_fragment[i/8] |= 1<<(i%8);
        }
        if(ts_util_test_bit(&value->mv_data[TS_KEY_SIZE_BITS - walk->offset], i)) {
            walk->current->mask[i/8] |= 1<<(i%8);
            walk->current->jumps[jumps] = &value[
                ((TS_KEY_SIZE_BITS - walk->offset) * 2) + 
                (sizeof(unsigned int) * jumps)];

            jumps++;
        }
    }

    mdb_txn_commit(txn);
}

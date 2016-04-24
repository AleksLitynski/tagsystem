#include "tstag.h"

#include <stdlib.h>


void ts_tag_create(ts_env * env, char * tag) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * meta_key, * meta_data, * root_keytag;
    ts_tag_metadata * meta;

    // create the meta (0) item, if it doesn't exist
    meta_key->mv_size = sizeof(unsigned int); 
    meta_key->mv_data = 0;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_CREATE | MDB_INTEGERKEY, dbi);
    int res = mdb_get(txn, *dbi, meta_key, meta_data);
    meta = (ts_tag_metadata*) meta_data->mv_data;
    if(res == MDB_NOTFOUND) {
        meta->rootId = 1;
        meta->nextId = 2;
        mdb_put(txn, *dbi, meta_key, meta_data, 0);
    }

    mdb_txn_commit(txn);
}

void _ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, char * tag, ts_node * node);
void ts_tag_insert(ts_env * env, char * tag, ts_doc_id * doc) {
    uint8_t mask[TS_KEY_SIZE_BYTES] = {0};
    ts_node node;
    MDB_txn * txn;
    MDB_val data[2];
    data[0].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    data[1].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    mdb_txn_begin(env->env, NULL, 0, &txn);
    node.key = 0;
    node.doc_id = (uint8_t *) doc;
    node.mask = mask;
    _ts_tag_move(txn, data, env, tag, &node);
    mdb_txn_commit(txn);
    free(data[0].mv_data);
    free(data[1].mv_data);

/*
typedef struct {
    int size;
    unsigned int key; // 0 means allocate a new one for me
    uint8_t * doc_id;
    uint8_t * mask;
    unsigned int * jumps;
} ts_node;
typedef uint8_t ts_node_cmp[TS_MAX_NODE_SIZE_BYTES];
*/


}

void _ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, char * tag, ts_node * node) {
    // variables
    MDB_dbi * dbi;
    MDB_val * key, * meta_data, * dbOut;
    ts_node * current;

    // setup transaction
    mdb_dbi_open(txn, tag, MDB_INTEGERKEY, dbi);
    key->mv_size = sizeof(unsigned int); 

    // get metadata
    key->mv_data = 0;
    mdb_get(txn, *dbi, key, meta_data);
    ts_tag_metadata * meta = (ts_tag_metadata*) meta_data->mv_data;

    // get root node
    key->mv_data = &meta->rootId;
    int res = mdb_get(txn, *dbi, key, dbOut);
    
    if(res == MDB_NOTFOUND) {
        node->key = *(int *) key->mv_data;

        ts_node_to_mdb_val(
            node, 
            TS_KEY_SIZE_BITS, 0, 
            0, 0, 
            &new_data[0]);

        mdb_put(txn, *dbi, key, &new_data[0], 0);
        free(new_data[0].mv_data);
        return;             
 
    } else {

        // there is a root. Walk the tree and insert ourself once the walk runs out
        current->key = (unsigned int) key->mv_data;
        ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS, current);

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
                key->mv_data = &current->jumps[maskCount];
                mdb_get(txn, * dbi, key, dbOut);  // overwrite current node
                current->key = (unsigned int) key->mv_data;
                ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS - bitIndex, current);

                nodeInset = bitIndex;           // increase the inset
                maskCount = 0;                  // reset the mask count
                continue;
            } else {
                //  neither branch exists. Insert the data

                // add the new node
                // copy the mask/jumps the user provided
                ts_node_to_mdb_val(
                    node, 
                    TS_KEY_SIZE_BITS - bitIndex, bitIndex, 
                    0, 0, 
                    &new_data[0]);
                if(node->key == 0) {
                    // if the user provided a key, use it
                    node->key = meta->nextId;
                    meta->nextId++;
                }
                key->mv_data = &node->key;
                mdb_put(txn, *dbi, key, &new_data[0], 0);

                // update parent mask
                int parentTotalSize = current->size + sizeof(unsigned int);
                int parentDataSize = TS_KEY_SIZE_BITS - nodeInset;

                // void ts_node_from_mdb_val(MDB_val * val, int id_size_bits, ts_node * node) {
                ts_node_to_mdb_val(
                    current, 
                    TS_KEY_SIZE_BITS - nodeInset, nodeInset, 
                    node->key, bitIndex - nodeInset, 
                    &new_data[1]);

                key->mv_data = &current->key;
                mdb_put(txn, *dbi, key, &new_data[1], 0);
                return;             
            }
        }
    }
}

void ts_tag_remove(ts_env * env, char * tag, ts_doc_id * doc) {
    // grab the tag tree
    // travsere to the node that ends with this document
    // migrate all the branches to the parent, except this node
    // (can't migrate to parent, need to... something)
    // remove this node

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, * meta_data, * dbOut;
    ts_node * current;
    MDB_val data[2];
    ts_tag_metadata * meta;
    data[0].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    data[1].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);

    // setup transaction
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_INTEGERKEY, dbi);
    key->mv_size = sizeof(unsigned int); 

    // get metadata
    key->mv_data = 0;
    mdb_get(txn, *dbi, key, meta_data);
    meta = (ts_tag_metadata *) meta_data->mv_data;

    
    // get root node
    key->mv_data = &meta->rootId;
    int res = mdb_get(txn, *dbi, key, dbOut);
    
    if(res == MDB_NOTFOUND) {
        // these is no root - our item has already been removed
        mdb_txn_commit(txn);
        return;             
    } else {

        // there is a root. Walk the tree to find our leaf
        current->key = (unsigned int) key->mv_data;
        ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS, current);

        // the depth of the current node
        int nodeInset = 0; 
        int maskCount = 0;
        unsigned int prevId = current->key;
        int prevMaskCount = 0;

        int bitIndex = 0;
        for(;bitIndex < TS_KEY_SIZE_BITS; bitIndex++) {
            int localIndex = bitIndex - nodeInset;
            uint8_t bitHasMask = ts_util_test_bit(current->mask, localIndex);
             
            if(ts_util_test_bit(*doc, bitIndex) == ts_util_test_bit(current->doc_id, localIndex)) {
                if(bitHasMask) maskCount++;
                //  this branch has it, continue
                continue; 
            } else if(bitHasMask) {
                prevId = current->key;
                prevMaskCount = maskCount;

                // the other branch has it, jump, then continue
                key->mv_data = &current->jumps[maskCount];
                mdb_get(txn, *dbi, key, dbOut);  // overwrite current node
                current->key = (unsigned int) key->mv_data;
                ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS - bitIndex, current);

                nodeInset = bitIndex;           // increase the inset
                maskCount = 0;                  // reset the mask count
                continue;
            } else {
                // we coudn't find this document in the tree
                mdb_txn_commit(txn);
                return;
            }
        }

        if(bitIndex >= TS_KEY_SIZE_BITS) {
            // remove from parent
            MDB_val * dbOutParent;
            key->mv_data = &prevId;
            mdb_get(txn, *dbi, key, dbOutParent); 
            // create a mask such that the jump will be turned off
            ((uint8_t*) dbOutParent->mv_data)[prevMaskCount/8] &= ~(1<<(prevMaskCount%8));
            mdb_put(txn, *dbi, key, dbOutParent, 0);

            // re-insert all children
            int jumpNum = 0;
            for(int i = 0; i < TS_KEY_SIZE_BITS - nodeInset; i++) {
                if(ts_util_test_bit(current->mask, i)) {
                                    
                    // I'm not sure if this will overwrite the value of dbOut before
                    //      the txn completes, of if it's ok.
                    key->mv_data = &current->jumps[jumpNum];
                    mdb_get(txn, *dbi, key, dbOut);

                    ts_node * dislocated_child;
                    ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS, dislocated_child);
                    _ts_tag_move(txn, data, env, tag, dislocated_child);

//void _ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_node * node);
                    jumpNum++;
                }
            }

            // remove from lmdb
            key->mv_data = &current->key;
            mdb_del(txn, *dbi, key, dbOut);
        
        }

        mdb_txn_commit(txn);
        free(data[0].mv_data);
        free(data[1].mv_data);
    }
}

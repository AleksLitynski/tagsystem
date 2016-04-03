#include "tstag.h"

#include <stdlib.h>


void ts_tag_create(ts_env * env, char * tagName, ts_tag * tag) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * meta_key, meta_data, root_keytag;

    tag->name = ts_util_concat("t", tag);

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
    ts_node node;
    MDB_txn * txn;
    MDB_val data[2];

    data[0].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    data[1].mv_data = malloc(TS_MAX_NODE_SIZE_BYTES);
    mdb_txn_begin(txn, &data, env->env, NULL, 0, &txn);
    node.key = 0;
    node.doc_id = doc;
    mode.mask = &mask;
    _ts_tag_move(env, tag, &node);
    mdb_txn_commit(&txn);
    free(data[0]->mv_data);
    free(data[1]->mv_data);

}

void _ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_node * node) {
    // variables
    MDB_dbi * dbi;
    MDB_val * key, meta, dbOut;
    ts_node current;

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

        ts_node_to_mdb_val(
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
                key->mv_data = current->jumps[maskCount];
                mdb_get(txn, dbi, key, dbOut);  // overwrite current node
                current.key = key->mv_data;
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
                    node->key = meta->mv_data.nextId;
                    meta->mv_data.nextId++;
                }
                key->mv_data = node->key;
                mdb_put(txn, dbi, key, &new_data[0]);

                // update parent mask
                int parentTotalSize = current->mv_size + sizeof(unsigned int);
                int parentDataSize = TS_KEY_SIZE_BITS - nodeInset;

                ts_node_from_mdb_val(
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

void ts_tag_remove(ts_env * env, ts_tag * tag, ts_doc_id * doc) {
    // grab the tag tree
    // travsere to the node that ends with this document
    // migrate all the branches to the parent, except this node
    // (can't migrate to parent, need to... something)
    // remove this node

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, meta, dbOut;
    ts_node current;
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
        ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS, current);

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
                ts_node_from_mdb_val(dbOut, TS_KEY_SIZE_BITS - bitIndex, current);

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
                    _ts_tag_move(txn, &data, env, key, dbOut);

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
}

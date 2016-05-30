#include "tstag.h"

#include <stdlib.h>
#include "stdio.h"

void tm2(char*p){
    char*x=malloc(1000000);
    printf("malloced: %s\n", p);
    free(x);
}

void _ts_tag_gen_meta(ts_env * env, char * tag) {
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_INTEGERKEY, &dbi);

    unsigned int zero = 0;
    MDB_val key = {.mv_size = sizeof(unsigned int), .mv_data = &zero}; 
    ts_tag_metadata meta = {
        .rootId = 1,
        .nextId = 2
    }; 
    MDB_val data = {
        .mv_size = sizeof(meta),
        .mv_data = &meta
    };
    // printf("a+b\n");
    int res = mdb_put(txn, dbi, &key, &data, 0);
    mdb_txn_commit(txn);
}

void ts_tag_create(ts_env * env, char * tag) {
    MDB_txn * txn;
    MDB_dbi dbi;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, tag, MDB_CREATE | MDB_INTEGERKEY, &dbi);

    unsigned int zero = 0;
    MDB_val key = {.mv_size = sizeof(unsigned int), .mv_data = &zero};
    MDB_val data;
    int res = mdb_get(txn, dbi, &key, &data);
    mdb_txn_commit(txn);
    
    if(res == MDB_NOTFOUND) _ts_tag_gen_meta(env, tag);
    
    // printf("ts_tag_create done\n");
}

void _ts_tag_move(ts_env * env, MDB_txn * txn, char * tag, ts_node * node );
void ts_tag_insert(ts_env * env, char * tag, ts_doc_id * doc) {

    printf("Inserting tag: %s\n------------------\n", tag);

    /*
    char * doc_str = ts_util_str_id(doc);
    char * doc_str_bin = ts_util_str_id_bin_split(doc, ' ', 8);
    printf("doc hex: %s\n", doc_str);
    printf("doc bin: %s\n", doc_str_bin);
    free(doc_str);
    free(doc_str_bin);
    */
    // create the inverted index DB for this
    //  tag if it doesn't exist
    ts_tag_create(env, tag);

    uint8_t mask[TS_ID_BYTES] = {0};
    ts_node node;
    MDB_txn * txn;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    node.key = 0;
    node.doc_id = (uint8_t *) doc;
    node.mask = mask;
    _ts_tag_move(env, txn, tag, &node);
    int res = mdb_txn_commit(txn);
    printf("txn res: %s\n", mdb_strerror(res));

}

void _ts_tag_move(ts_env * env, MDB_txn * txn, char * tag, ts_node * node){

    // setup dbi
    MDB_dbi dbi;
    mdb_dbi_open(txn, tag, MDB_INTEGERKEY, &dbi);

    // get metadata
    unsigned int meta_idx = 0;
    MDB_val key = { .mv_size = sizeof(unsigned int), .mv_data = &meta_idx};
    MDB_val meta_data;
    mdb_get(txn, dbi, &key, &meta_data);
    ts_tag_metadata * meta = meta_data.mv_data;
    printf("rootid: %i, nextid: %i\n", meta->rootId, meta->nextId);

    // get root node
    key.mv_data = &(meta->rootId);
    MDB_val dbOut;
    printf("getting from: %i %i\n", key.mv_size, *(unsigned int *)key.mv_data);
    int res = mdb_get(txn, dbi, &key, &dbOut);
    printf("get res: %s\n", mdb_strerror(res));
    
    // if no root, insert the whole thing at the root and exit
    if(res == MDB_NOTFOUND) {
        printf("No root, creating root\n");
        
        node->key = meta->rootId;

        MDB_val new_data = {
            .mv_size = 0, 
            .mv_data = calloc(TS_NODE_BYTES, 1)
        };
        ts_node_to_mdb_val(node, TS_ID_BITS, 0, 0, 0, &new_data);
        printf("insert size: %i\n", new_data.mv_data);
        printf("Inserting to: %i %i\n", key.mv_size, *(unsigned int *)key.mv_data);
        printf("Data size: %i\n", new_data.mv_size);
        int rez = mdb_put(txn, dbi, &key, &new_data, 0);
        printf("put res: %s\n", mdb_strerror(rez));
        free(new_data.mv_data);
        return;             
    }
    printf("Root existed\n");

    // there is a root. Walk the tree and insert ourself once the walk runs out
    ts_node current;
    current.key = (unsigned int) key.mv_data;
    ts_node_from_mdb_val(&dbOut, TS_ID_BITS, &current);

    // the depth of the current node
    int nodeInset = 0; 
    int maskCount = 0;

    for(int bitIndex = 0; bitIndex < TS_ID_BITS; bitIndex++) {
        int localIndex = bitIndex - nodeInset;
        uint8_t bitHasMask = ts_util_test_bit(current.mask, localIndex);

        printf("val: %i\n", ts_util_test_bit(current.doc_id, localIndex)); 
         
        if(ts_util_test_bit(node->doc_id, bitIndex) == ts_util_test_bit(current.doc_id, localIndex)) {
            if(bitHasMask) maskCount++;
            //  this branch has it, continue
            continue; 
        } else if(bitHasMask) {
            // the other branch has it, jump, then continue
            key.mv_data = &current.jumps[maskCount];
            mdb_get(txn, dbi, &key, &dbOut);  // overwrite current node
            current.key = (unsigned int) key.mv_data;
            ts_node_from_mdb_val(&dbOut, TS_ID_BITS - bitIndex, &current);

            nodeInset = bitIndex;           // increase the inset
            maskCount = 0;                  // reset the mask count
            continue;
        } else {
            //  neither branch exists. Insert the data

            // add the new node
            // copy the mask/jumps the user provided
            MDB_val new_data = {.mv_size = 0, .mv_data = malloc(TS_NODE_BYTES)};
            ts_node_to_mdb_val(
                node, 
                TS_ID_BITS - bitIndex, bitIndex, 
                0, 0, 
                &new_data);
            if(node->key == 0) {
                // if the user provided a key, use it 
                //  (if 0, they did not provide a key, so take from meta)
                node->key = meta->nextId;
                // and increment the meta 'next key'
                meta->nextId++;
                key.mv_data = &meta_idx;
                mdb_put(txn, dbi, &key, &meta_data, 0);
            }
            key.mv_data = &node->key;
            mdb_put(txn, dbi, &key, &new_data, 0);
            free(new_data.mv_data);

            // update parent mask
            MDB_val parent_data = {.mv_size = 0, .mv_data = malloc(TS_NODE_BYTES)};
            ts_node_to_mdb_val(
                &current, 
                TS_ID_BITS - nodeInset, nodeInset, 
                node->key, bitIndex - nodeInset, 
                &parent_data);

            key.mv_data = &current.key;
            mdb_put(txn, dbi, &key, &parent_data, 0);
            free(parent_data.mv_data);
            return;             
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
    ts_tag_metadata * meta;

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
        ts_node_from_mdb_val(dbOut, TS_ID_BITS, current);

        // the depth of the current node
        int nodeInset = 0; 
        int maskCount = 0;
        unsigned int prevId = current->key;
        int prevMaskCount = 0;

        int bitIndex = 0;
        for(;bitIndex < TS_ID_BITS; bitIndex++) {
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
                ts_node_from_mdb_val(dbOut, TS_ID_BITS - bitIndex, current);

                nodeInset = bitIndex;           // increase the inset
                maskCount = 0;                  // reset the mask count
                continue;
            } else {
                // we coudn't find this document in the tree
                mdb_txn_commit(txn);
                return;
            }
        }

        if(bitIndex >= TS_ID_BITS) {
            // remove from parent
            MDB_val * dbOutParent;
            key->mv_data = &prevId;
            mdb_get(txn, *dbi, key, dbOutParent); 
            // create a mask such that the jump will be turned off
            ((uint8_t*) dbOutParent->mv_data)[prevMaskCount/8] &= ~(1<<(prevMaskCount%8));
            mdb_put(txn, *dbi, key, dbOutParent, 0);

            // re-insert all children
            int jumpNum = 0;
            for(int i = 0; i < TS_ID_BITS - nodeInset; i++) {
                if(ts_util_test_bit(current->mask, i)) {
                                    
                    // I'm not sure if this will overwrite the value of dbOut before
                    //      the txn completes, of if it's ok.
                    key->mv_data = &current->jumps[jumpNum];
                    mdb_get(txn, *dbi, key, dbOut);

                    ts_node * dislocated_child;
                    ts_node_from_mdb_val(dbOut, TS_ID_BITS, dislocated_child);
                    _ts_tag_move(env, txn, tag, dislocated_child);

//void _ts_tag_move(MDB_txn * txn, MDB_val * new_data, ts_env * env, ts_tag * tag, ts_node * node);
                    jumpNum++;
                }
            }

            // remove from lmdb
            key->mv_data = &current->key;
            mdb_del(txn, *dbi, key, dbOut);
        
        }

        mdb_txn_commit(txn);
    }
}

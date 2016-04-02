#include "tsdoc.h"

#include <stdlib.h>
#include "tsutil.h"
#include "tstag.h"


void ts_doc_create(ts_env * env, ts_doc_id * id) {
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

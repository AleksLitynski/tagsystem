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
    mdb_dbi_open(txn, db_name, MDB_CREATE | MDB_INTEGERKEY, &dbi);
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
    // get the metadata
    // get the root
    // itterate to the edge. If you can't reach the edge, 
    //      insert a jump + new node to represent this data
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, meta, current, newNode;
    unsigned int currentKey;

    key.mv_size = sizeof(unsigned int); 
    key.mv_data = 0;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, db_name, MDB_INTEGERKEY, &dbi);
    mdb_get(txn, dbi, meta_key, meta);

    key->mv_data = meta->mv_data.rootId;
    currentKey = key->mv_data;
    int res = mdb_get(txn, dbi, key, current);
    
    if(res == MDB_NOTFOUND) {
        // there is no root. Make ourselves root and exit
        uint8_t data[40] = {0}; // array of 160 * 2 zeros 
        for(int i = 0; i < 20; i++) {
            data[i] = doc[i]; // copy 160 bit of the docs id
        }
        current->mv_size = 160 * 2; // doc-id and mask
        current->mv_data = data;
        mdb_put(txn, dbi, key, current);
    } else {
        // there is a root. Walk the tree and insert ourself once the walk runs out
        
        int maskCount = 0;
        int nodeInset = 0; // each time we jump, the node gets shorter. This is the amount
        for(int bitIndex = 0; bitIndex < 160; bitIndex++) {
            int localIndex = bitIndex - nodeInset;
            uint8_t bitHasMask = ts_util_test_bit(&current->mv_data[20], localIndex);
            if(bitHasMask) maskCount++;
             
            if(ts_util_test_bit(doc, bitIndex) ==
                    ts_util_test_bit(current->mv_data, localIndex)) {
                //  this branch has it, continue
                continue; 
            } else if(bitHasMask) {
                // the other branch has it, jump, then continue
                key->mv_data = current->mv_data[40 + maskCount * sizeof(unsigned int)];
                mdb_get(txn, dbi, key, current); // overwrite current node
                currentKey = key->mv_data;
                nodeInset = bitIndex; // increase the inset
                maskCount = 0; // reset the mask count
                continue;
            } else {
                //  neither branch exists. Insert the data
                int remainingBits = bitIndex;
                uint8_t * nodeData = calloc(sizeof(uint8_t) * (160 - bitIndex) * 2);
                nodeDataUsed = 1;
                for(int i = bitIndex + 1; i < 160; i++) {
                    if(ts_util_test_bit(doc, i)) {
                        nodeData[i/8] &= 1 << (i%8);
                    }
                }
                
                // insert new data
                key->mv_data = meta->mv_data.nextId;
                unsigned int newKey = key->mv_data;
                meta->mv_data.nextId++;
                newNode->mv_size = sizeof(uint8_t) * (160 - bitIndex);
                newNode->mv_data = nodeData;
                mdb_put(txn, dbi, key, newNode);

                // update parent node
                int parentTotalSize = current->mv_size + sizeof(unsigned int);
                int parentDataSize = 160 - nodeInset;
                uint8_t * parentNode = malloc(parentTotalSize);
                for(int i = 0; i < parentDataSize; i++) {
                    if(ts_util_test_bit(current->mv_data, i) ||
                        // if we're in the mask, at the branch we are creating, set the flag
                        i == parentSize + bitIndex) {
                        parentData[i/8] &= 1 << (i%8);
                    }
                }

                int newJumpIndex = 0;
                int oldJumpIndex = 0;
                for(int i = parentDataSize; i < parentDataSize * 2; i++) {
                    if(ts_util_test_bit(current->mv_data, i) {
                        parentData[i/8] &= 1 << (i%8);     
                        parentData[parentDataSize + (newJumpIndex * sizeof(unsigned int))] = 
                            current->mv_data[parentDataSize + (oldJumpIndex * sizeof(unsigned int))];
                        newJumpIndex++;
                        oldJumpIndex++;
                    } 

                    if(i == parentSize + bitIndex) {
                        parentData[parentDataSize + (newJumpIndex * sizeof(unsigned int))] =
                           newKey;
                        newJumpIndex++;
                    }
                }

                key->mv_data = currentKey;
                current->mv_size = parentTotalSize;
                current->mv_data = parentData;
                mdb_put(txn, dbi, key, current);

                mdb_txn_commit(txn);
                free(nodeData);
                free(parentNode);
                return;             
            }
        }
    }

    //if we make it to here, the item was already in the tree
    mdb_txn_commit(txn);
}


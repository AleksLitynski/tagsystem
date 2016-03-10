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
    return ts_env_create(menv, prefix);
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
    ts_env_close(env);
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

void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tag) {
    // go-to iIndex db
    // get the tag item. Make it if it doesn't exist
    // ... insert the doc
    // go-to index, get doc, add tag
    ts_tagtree * root;
    _ts_doc_tag_get_root(env, tag, root);

    if(root == 0){
        MDB_val newNode;
        _ts_doc_tag_make_node(doc, 0, &newNode);
        _ts_doc_tag_set_root(env, tag, &(newNode.mv_data));
    } else {
        // iterate through. If we get to the end,
        //      it's already set. All done.
        //      If we don't get to the end, 
        //          add a jump (do jump compact/allocate logic)
        //          then link up the new node to the jump
    }

}

void _ts_doc_tag_set_idx(ts_env * env, ts_doc_id * doc, char * tag){
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, val;

    key.mv_size = strlen(doc);
    key.mv_data = doc;
    val.mv_size = strlen(tag);
    val.mv_data = tag;

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->index, MDB_CREATE, &dbi);
    int res = mdb_get(txn, dbi, key, data);

    // write the new pointer over the old one
    mdb_txn_commit(txn);
}
}

void _ts_doc_tag_make_node(char * doc, int totalBitIn, MDB_val * node) {
    int byteIn = (totalBitIn + 4) / 8;
    int byteLeft = 20 - byteIn;
    int bitIn = totalBitIn % 8; // position into the current byte
    int bitLeft = 8 - bitIn; // bits left in the current byte
    int totalBitLeft = 160 - totalBitIn;

    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, val;
    key.mv_size = strlen(tag);
    key.mv_data = tag;
    val.mv_size =
        (sizeof(uint8_t) * byteLeft * 2) +  // path + mask
        (sizeof(uint8_t) * 2);              // path + jump count
    val.mv_data = 0;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->iIndex, 0, &dbi);
    mdb_put(txn, dbi, key, val); // allocate the required amount of data
    mdb_get(txn, dbi, key, val); // get a pointer to the data
    mdb_txn_commit(txn);

    uint8_t * data = &(val.mv_data);
    data[0] = totalBitLeft; // number if bytes in the path/mask
    tData[1] = 0;   // number of blanks left for jumps

    // iterate bytes
    for(int i = byteIn; i < 20; i++) {
        int dataIdx = i - byteIn + 2;
        data[dataIdx + totalBitLeft] = 0;
        // iterate bits
        for(int j = bitIn; j < 8; j++) {
            uint8_t mask = 1 << j; 
            data[dataIdx] &= mask & doc[i];
        }
    }

    node = &val;
}

void _ts_doc_tag_get_root(ts_env * env, char * tag, ts_tagtree * firstItem) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, data;

    key.mv_size = strlen(tag);
    key.mv_data = tag;

    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->iIndex, MDB_CREATE, &dbi);

    int res = mdb_get(txn, dbi, key, data);
    if(res != MDB_NOTFOUND) {
        firstItem = data.mv_data;
    } else {
        firstItem = (ts_tagtree *)0;
        mdb_put(txn, dbi, key, firstItem);
    }
    mdb_txn_commit(txn);
}

void _ts_doc_tag_set_root(ts_env * env, char * tag, ts_tagtree * firstItem) {
    MDB_txn * txn;
    MDB_dbi * dbi;
    MDB_val * key, data;
    key.mv_size = strlen(tag);
    key.mv_data = tag;
    mdb_txn_begin(env->env, NULL, 0, &txn);
    mdb_dbi_open(txn, env->iIndex, 0, &dbi);
    int res = mdb_get(txn, dbi, key, data);

    // write the new pointer over the old one
    data.mv_data = firstItem;
    mdb_txn_commit(txn);
}

#include "../src/ts.h"

int main(int argc, char * argv[]) {
    return 1;

/*
    MDB_env *menv;
    mdb_env_create(&menv);
    mdb_env_set_maxreaders(menv, 1);
    mdb_env_set_mapsize(menv, 10485760);
    mdb_env_open(menv, "test_db.mdb", mdb_fixedmap, 0664);
    mdb_open_file(env, "test_db.mdb");
    
    ts_env *tenv;
    ts_init(env, tenv, "test1");

    ts_doc_id doc1 = ts_add_doc(tenv, "hello, I am a document");
    ts_doc_id doc2 = ts_add_doc(tenv, "I am a different document");

    ts_add_tag(tenv, doc1, "a");
    ts_add_tag(tenv, doc1, "b");
    ts_add_tag(tenv, doc1, "c");

    ts_add_tag(tenv, doc2, "b");
    ts_add_tag(tenv, doc2, "c");
    ts_add_tag(tenv, doc2, "d");

    ts_tags res1 = ts_itterate(tenv, 0, (char[]){"a", "b", "c"});
    ts_tags res2 = ts_itterate(tenv, 0, (char[]){"b", "c"});
    ts_tags res3 = ts_itterate(tenv, 0, (char[]){"b", "c", "d"});
    
    ts_rem_tag(tenv, doc1, "a");
    ts_rem_tag(tenv, doc1, "c");
    ts_tags res4 = ts_iterate(tenv, 0, (char[]){"a", "b", "c"});
    ts_tags res5 = ts_iterate(tenv, 0, (char[]){"b"});

    ts_rem_doc(tenv, doc2);
    ts_tags res6 = ts_iterate(tenv, 0, (char[]){"c"});
    
    mdb_env_close(env);

    */
    return 0;

}

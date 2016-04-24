#include <stdio.h>
#include "ts.h"
#include "lmdb.h"
#include "openssl/sha.h"

int main(int argc, char * argv[]) {
    
    mdb_test();
    std_test();
    ssl_test();

    ts_create_test();
    
    return 0;
}

int mdb_test() {
    MDB_env * env;
    mdb_env_create(&env);
    mdb_env_close(env);
    return 1;
}

int std_test() {
    printf("hey");
    return 1;
}

int ssl_test() {
    const unsigned char * src = "abcd";
    unsigned char * dest;
    SHA1(src, 4, dest);
    printf("%s", dest);
    return 1;
}

int ts_create_test() {
    iter(ts_ls, args(), doc, {

    })

    ts_cws("./testdata");

    ts_cs(ts_mk());
    ts_tag("doc_a");
    ts_tag("doc");
    ts_cs(ts_mk());
    ts_tag("doc_b");
    ts_tag("doc");

    printf(ts_pws());


    printf("stuff with 'doc' tag");
    iter(ts_ls, args("--doc"), doc, {
        printf(doc);
    })

    printf("stuff with 'doc_a' tag");
    iter(ts_ls, args("--doc_a"), doc, {
        printf(doc);
    })

    printf("stuff with the 'doc_b' and 'doc' tag");
    iter(ts_ls, args("-doc_a+doc+doc_b"), doc, {
        printf(doc);
    })

    // I forgot about removing docs/tags!


 


}

    

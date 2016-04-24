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

    ts_cws("./testdata");

    free(ts_mk("doc+a"));
    ts_cs("--");
    free(ts_mk("doc+b"));
    ts_tag("doc_b", "--doc+b");

    printf(ts_pws());

    printf("stuff with 'doc' tag");
    iter(ts_ls, args("--doc"), doc, {
        printf(doc);
    })

    printf("stuff with 'doc' and 'a' tag");
    iter(ts_ls, args("--doc+a"), doc, {
        printf(doc);
    })

    printf("stuff with the 'b' and not 'a' tag");
    iter(ts_ls, args("-a+b"), doc, {
        printf(doc);
    })

    // I forgot about removing docs/tags!


    ts_rm("--doc+b");
    ts_untag("doc", "a"); // remove doc tag from stuff with a tag

    ts_rm("--a"); // remove everything with a


}

    

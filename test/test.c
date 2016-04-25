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

    ts_cs("doc+a");
    ts_mk0();
    ts_cs("--doc+b");
    ts_mk();
    ts_cs("--doc+b");
    ts_tag("doc_b");

    printf(ts_pws());

    printf("stuff with 'doc' tag");
    ts_cs("--doc");
    iter0(ts_ls, doc, {
        printf(doc);
    })

    printf("stuff with 'doc' and 'a' tag");
    ts_cs("--doc+a");
    iter0(ts_ls, doc, {
        printf(doc);
    })

    printf("stuff with the 'b' and not 'a' tag");
    ts_cs("-a+b");
    iter0(ts_ls, doc, {
        printf(doc);
        char * path = ts_resolve(doc);
        printf("file path: %s", path);
        free(path);
    })


    ts_cs("--doc+b");
    ts_rm();

    ts_cs("--a");
    ts_untag("doc"); // remove doc tag from stuff with 'a' tag

    ts_cs("--a");
    ts_rm(); // remove everything with a

}

    

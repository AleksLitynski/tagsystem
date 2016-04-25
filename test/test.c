#include <stdio.h>
#include "ts.h"
#include "tsiter.h"

int ts_create_test() {

    ts_cws("./testdata");

    ts_cs("doc+a");
    ts_mk0();
    ts_cs("--doc+b");
    ts_mk0();
    ts_cs("--doc+b");
    ts_tag("doc_b");

    printf(ts_pws()); printf("\n");

    printf("stuff with 'doc' tag\n");
    ts_cs("--doc");
    iter0(ts_ls, doc, {
        printf("%s\n", doc);
    })

    printf("stuff with 'doc' and 'a' tag\n");
    ts_cs("--doc+a");
    iter0(ts_ls, doc, {
        printf("%s\n", doc);
    })

    printf("stuff with the 'b' and not 'a' tag\n");
    ts_cs("-a+b");
    iter0(ts_ls, doc, {
        printf("%s\n", doc);
        char * path = ts_resolve(doc);
        printf("file path: %s\n", path);
        free(path);
    })


    ts_cs("--doc+b");
    ts_rm();

    ts_cs("--a");
    ts_untag("doc"); // remove doc tag from stuff with 'a' tag

    ts_cs("--a");
    ts_rm(); // remove everything with a

}

    
int main(int argc, char * argv[]) {
    
    // ts_create_test();
    
    return 0;
}



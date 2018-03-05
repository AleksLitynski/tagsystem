#include "test.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "../lib/cmocka/include/cmocka.h"
#include "../lib/sds/sds.h"
#include "../lib/fs.c/fs.h"
#include "../src/tsdb.h"

void hello_world_test(void **state) {
    // https://git.cryptomilk.org/projects/cmocka.git/tree/example
}

// tsdb
void tsdb_test(void ** state) {

}


int main(void) {

    printf("%s", hello_world());

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(hello_world_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

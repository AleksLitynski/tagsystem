#include "ts.h"

/*int main(int argc, char * argv[]) {
    return 0;
}*/
    ts_ls  // output the docs in the current set (either by id or file path)
    ts_cs  // change the current set. +a+b+c to add a, b, c. a+b+c to set to a, b, c. -b+c to remove b, add c
    ts_tag // tag to a file by it's id or filename. Or, tag all files in pws
    ts_mk  // make a file

    ts_cts // Change tag system. Stored in $TTSPATH
    ts_pws // print the present working set. Stored in $TSPWS


int ts_ls(



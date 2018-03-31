#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "main.h"
#include "parsing.h"

int main(int argc, char * argv[]) {

    if(argc <= 1) return help_cmd(argc, argv);

    int (*op)(int, char**) = 
        matches("list", argv[1])        ? list_cmd:
        matches("make", argv[1])        ? make_cmd:
        matches("remove", argv[1])      ? remove_cmd:
        matches("tag", argv[1])         ? tag_cmd:
        matches("changeset", argv[1])   ? changeset_cmd:
        matches("cs", argv[1])          ? changeset_cmd:
        matches("presentset", argv[1])  ? presentset_cmd:
        matches("pws", argv[1])         ? presentset_cmd:
        matches("help", argv[1])        ? help_cmd:
        matches("-help", argv[1])       ? help_cmd:
        matches("--help", argv[1])      ? help_cmd:
                                          help_cmd;
    
    return op(argc - 2, &argv[2]);
}


int list_cmd(int argc, char * argv[]) {

    arg_list args;
    args_create(&args, 2);
    bool * show_id = args_add_bool(&args, "id");
    bool * preview_path = args_add_bool(&args, "preview");
    args_parse(&args, argc, argv);

    // use show_id and preview_path


    args_close(&args);
    return EXIT_SUCCESS;
}

int make_cmd(int argc, char * argv[]) {
    // id or path

    return EXIT_SUCCESS;
}


int remove_cmd(int argc, char * argv[]) {
    // id or path
    // confirm delete many


    return EXIT_SUCCESS;
}


int tag_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    return EXIT_SUCCESS;
}

int changeset_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    return EXIT_SUCCESS;
}

int presentset_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    return EXIT_SUCCESS;
}

int help_cmd(int argc, char * argv[]) {
    printf("Do it right next time\n");
    return EXIT_SUCCESS;
}

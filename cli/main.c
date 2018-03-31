#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "main.h"
#include "parsing.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

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

ts_db * get_db() {
    char * db_path = getenv("TSDBPATH");
    if(db_path == 0) db_path = "~/.tsysdb";
    ts_db * db = malloc(sizeof(ts_db));
    ts_db_open(db, db_path);
    return db;
}

int list_cmd(int argc, char * argv[]) {

    arg_list args;
    args_create(&args, 1);
    bool ** show_id = args_add_bool(&args, "id");
    int last = args_parse(&args, argc, argv);

    sds new_set = concat_string(sdsempty(), argc - last, &argv[last]);
    
    char * current_set = getenv("TSPWS");
    hash_t * set = hash_new();
    if(current_set != 0) tag_set(set, current_set);
    tag_set(set, new_set);

    ts_db * db = get_db();

    int tags_count = hash_size(set);
    ts_tags_readonly * tags = malloc(sizeof(ts_tags) * tags_count);
    int i = 0;
    MDB_txn * txn;
    hash_each(set, {
        ts_tags_open_readonly(&tags[i], db, key, &txn);
        i++;
    })
    mdb_txn_commit(txn);

    // use show_id and preview_path
    ts_search search;
    ts_search_create(&search, tags->tags, tags_count);
    
    ts_id id;
    while(ts_search_next(&search, &id) != TS_SEARCH_DONE) {
        // print the id or doc
        if(**show_id) {
            sds id_str = ts_id_string(&id, sdsempty());
            printf("%s\n", &id_str);
            sdsfree(id_str);
        } else {
            // make a doc, print the path
            ts_doc doc;
            ts_doc_open(&doc, db, id);
            printf("%s\n", doc.path);
            ts_doc_close(&doc);
        }
    }

    ts_search_close(&search);


    free(tags);
    ts_db_close(db);
    sdsfree(current_set);
    tag_set_free(set);

    args_close(&args);
    return EXIT_SUCCESS;
}

int make_cmd(int argc, char * argv[]) {
    // id or path
    printf("make called\n");


    return EXIT_SUCCESS;
}


int remove_cmd(int argc, char * argv[]) {
    // id or path
    // confirm delete many


    printf("rm called\n");
    return EXIT_SUCCESS;
}


int tag_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    printf("tag called\n");
    return EXIT_SUCCESS;
}

int changeset_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    printf("changeset called\n");
    return EXIT_SUCCESS;
}

int presentset_cmd(int argc, char * argv[]) {

    // either docs to tag, and/or take docs from stdin

    printf("presentset called\n");
    return EXIT_SUCCESS;
}

int help_cmd(int argc, char * argv[]) {
    printf("Do it right next time\n");
    return EXIT_SUCCESS;
}

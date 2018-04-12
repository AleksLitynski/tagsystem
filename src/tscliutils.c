#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <libgen.h>
#include "hash.h"
#include "sds.h"
#include "tscliutils.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tstagset.h"
#include "tsargs.h"


ts_cli_ctx * ts_cli_ctx_open() {
    ts_cli_ctx * self = malloc(sizeof(ts_cli_ctx));

    char * db_path = getenv("TSDBPATH");
    if(db_path == 0) db_path = "~/.tsysdb";
    self->db = malloc(sizeof(ts_db));
    ts_db_open(self->db, db_path);

    self->pws = hash_new();
    char * current_tags_str = getenv("TSPWS");
    if(current_tags_str != 0) ts_tagset_create(self->pws, current_tags_str);
    return self;

}

int ts_cli_ctx_close(ts_cli_ctx * self) {
    free(self->db);
    hash_free(self->pws);
    free(self);
}

void ts_cli_save_ctx(ts_cli_ctx * ctx) {
    sds pws_str = ts_cli_print_pws(ctx);
    setenv("TSPWS", pws_str, true);
    sdsfree(pws_str);
}

void ts_cli_print_id(ts_cli_ctx * ctx, ts_id * id, bool show_id) {
    // print the id or doc
    if(show_id) {
        sds id_str = ts_id_string(id, sdsempty());
        printf("%s\n", &id_str);
        sdsfree(id_str);
    } else {
        // make a doc, print the path
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, *id);
        printf("%s\n", doc.path);
        ts_doc_close(&doc);
    }
}

sds ts_cli_print_pws(ts_cli_ctx *  ctx) {
    sds pws_str = sdsempty();
    hash_each(ctx->pws, {
        pws_str = sdscat(pws_str, "+");
        pws_str = sdscat(pws_str, key);
    })
    return pws_str;
}

bool ts_cli_confirm(char * message) {
    char input[100] = {0};
    while(!ts_args_matches("yes", input) && !ts_args_matches("no", input)) {
        printf("%s [y/n]\n", message);
        fgets(input, 100, stdin);   
    }

    return ts_args_matches("yes", input);
}

sds * ts_cli_stdin_to_array(int * count) {
    sds input = sdsempty();

    char next[1000] = {0};
    while(fgets(next, 1000, stdin)) {
        input = sdscat(input, next);
    }

    return sdssplitlen(input, sdslen(input), "\n", 1, count);
}

char * ts_cli_doc_path_id(sds doc_path) {
    char * doc_path_dup = strdup(doc_path);
    char * doc_name_temp = dirname(doc_path_dup);
    char * doc_name = strdup(doc_name_temp);
    free(doc_path_dup);
    free(doc_name_temp);
    return doc_name;
}

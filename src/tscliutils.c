#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
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

    char * tsdbpath = getenv("TSDBPATH");
    if(tsdbpath == 0) tsdbpath = "";
    char * xdghome = getenv("XDG_CONFIG_HOME");
    if(xdghome == 0) xdghome = "";
    char * homepath = getenv("HOME");
    if(homepath == 0) homepath = "";


    sds db_path = sdscat(sdsempty(), tsdbpath);
    if(sdslen(db_path) == 0) {
        sds home = sdscat(sdsempty(), xdghome);
        if(sdslen(home) == 0) home = sdscat(home, homepath);
        if(sdslen(home) == 0) {
            struct passwd * pswd = getpwuid(geteuid());
            home = sdscat(home, pswd->pw_dir);
        }
        db_path = sdscatsds(db_path, home);
        db_path = sdscat(db_path, "/.tsysdb");
        sdsfree(home);
    }

    self->db = malloc(sizeof(ts_db));
    ts_db_open(self->db, db_path);

    self->in = stdin;
    self->out = stdout;
    sdsfree(db_path);

    return self;
}

int ts_cli_ctx_close(ts_cli_ctx * self) {
    free(self->db);
    free(self);
}

void ts_cli_print_id(ts_cli_ctx * ctx, ts_id * id, bool show_id) {
    // print the id or doc
    if(show_id) {
        sds id_str = ts_id_string(id, sdsempty());
        fprintf(ctx->out, "%s\n", id_str);
        sdsfree(id_str);
    } else {
        // make a doc, print the path
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, *id);
        fprintf(ctx->out, "%s\n", doc.path);
        ts_doc_close(&doc);
    }
}

bool ts_cli_confirm(ts_cli_ctx * ctx, char * message) {
    char input[100] = {0};
    while(!ts_args_matches("yes", input) && !ts_args_matches("no", input)) {
        fprintf(ctx->out, "%s [y/n]\n", message);
        fgets(input, 100, ctx->in);   
    }

    return ts_args_matches("yes", input);
}

sds * ts_cli_stdin_to_array(ts_cli_ctx * ctx, int * count) {
    sds input = sdsempty();

    char next[1000] = {0};
    while(fgets(next, 1000, ctx->in)) {
        input = sdscat(input, next);
    }

    return sdssplitlen(input, sdslen(input), "\n", 1, count);
}

char * ts_cli_doc_path_id(sds doc_path) {
    char * doc_path_dup = strdup(doc_path);
    char * doc_name_temp = basename(doc_path_dup);
    char * doc_name = strdup(doc_name_temp);
    free(doc_path_dup);
    return doc_name;
}

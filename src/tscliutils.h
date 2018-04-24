#pragma once

// includes
#include "hash.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include <stdio.h>

// macros

// types
typedef struct {
    ts_db * db;
    FILE * in;
    FILE * out;
} ts_cli_ctx;

// functions
void ts_cli_print_id(ts_cli_ctx * ctx, ts_id * id, bool show_id);
void ts_cli_print_tags(ts_cli_ctx * ctx, ts_id * id);
bool ts_cli_confirm(ts_cli_ctx * ctx, char * message);
sds * ts_cli_stdin_to_array(ts_cli_ctx * ctx, int * count);
char * ts_cli_doc_path_id(sds doc_path);

ts_cli_ctx * ts_cli_ctx_open();
int ts_cli_ctx_close();

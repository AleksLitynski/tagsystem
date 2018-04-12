#pragma once

// includes
#include "hash.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

// macros

// types
typedef struct {
    ts_db * db;
    hash_t * pws;
} ts_cli_ctx;

// functions
void ts_cli_print_id(ts_cli_ctx * ctx, ts_id * id, bool show_id);
bool ts_cli_confirm(char * message);
sds ts_cli_print_pws(ts_cli_ctx *  ctx);
sds * ts_cli_stdin_to_array(int * count);
char * ts_cli_doc_path_id(sds doc_path);

ts_cli_ctx * ts_cli_ctx_open();
int ts_cli_ctx_close();
void ts_cli_save_ctx(ts_cli_ctx * ctx);

#pragma once

// includes
#include "hash.h"
#include "parsing.h"
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
} cli_ctx;

// functions
ts_db * get_db();
hash_t * get_working_set();
ts_search * search_set(cli_ctx * ctx);
void search_set_close(ts_search * search);
void print_id(cli_ctx * ctx, ts_id * id, bool show_id);
void save_pws(cli_ctx * ctx);
bool set_has_one_item(cli_ctx * ctx);
bool confirm(char * message);
sds print_pws(cli_ctx *  ctx);
sds * stdin_to_array(int * count);
char * doc_path_id(sds doc_path);
#pragma once

// includes
#include "tscli.h"
#include <stdio.h>

// macros

// types

// functions
int ts_cli_help(ts_cli_ctx * ctx, int argc, char * argv[]);

int ts_cli_list_help(ts_cli_ctx * ctx);
int ts_cli_make_help(ts_cli_ctx * ctx);
int ts_cli_remove_help(ts_cli_ctx * ctx);
int ts_cli_tag_help(ts_cli_ctx * ctx);
int ts_cli_changeset_help(ts_cli_ctx * ctx);
int ts_cli_presentset_help(ts_cli_ctx * ctx);
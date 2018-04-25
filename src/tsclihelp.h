#pragma once

// includes
#include "tscli.h"
#include <stdio.h>

// macros

// types

// functions
// help text for the main cli program
int ts_cli_help(ts_cli_ctx * ctx, int argc, char * argv[]);

// help text for the cli subcommands
int ts_cli_list_help(ts_cli_ctx * ctx);
int ts_cli_make_help(ts_cli_ctx * ctx);
int ts_cli_remove_help(ts_cli_ctx * ctx);
int ts_cli_tag_help(ts_cli_ctx * ctx);
int ts_cli_changeset_help(ts_cli_ctx * ctx);
int ts_cli_presentset_help(ts_cli_ctx * ctx);
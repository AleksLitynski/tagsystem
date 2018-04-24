#pragma once

// includes
#include "hash.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tscliutils.h"
#include "tsclihelp.h"

// macros
// a helpful way to make cli calls programatically
#define CLI(ctx, op, ...) {                        \
    char * args[] = { __VA_ARGS__ };               \
    op(ctx, sizeof(args) / sizeof(char *), args);  \
}

// types

// functions
int ts_cli_list(ts_cli_ctx * ctx, int argc, char * argv[]);
int ts_cli_make(ts_cli_ctx * ctx, int argc, char * argv[]);
int ts_cli_remove(ts_cli_ctx * ctx, int argc, char * argv[]);
int ts_cli_tag(ts_cli_ctx * ctx, int argc, char * argv[]);
int ts_cli_changeset(ts_cli_ctx * ctx, int argc, char * argv[]);
int ts_cli_presentset(ts_cli_ctx * ctx, int argc, char * argv[]);
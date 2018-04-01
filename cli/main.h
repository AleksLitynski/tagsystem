#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"
#include "utils.h"

// macros

// types


// functions
int main(int argc, char * argv[]);

int list_cmd(cli_ctx * ctx, int argc, char * argv[]);
int make_cmd(cli_ctx * ctx, int argc, char * argv[]);
int remove_cmd(cli_ctx * ctx, int argc, char * argv[]);
int tag_cmd(cli_ctx * ctx, int argc, char * argv[]);
int changeset_cmd(cli_ctx * ctx, int argc, char * argv[]);
int presentset_cmd(cli_ctx * ctx, int argc, char * argv[]);
int help_cmd(cli_ctx * ctx, int argc, char * argv[]);
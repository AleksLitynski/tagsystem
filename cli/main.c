#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "main.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tscli.h"
#include "tsargs.h"

int main(int argc, char * argv[]) {

    ts_cli_ctx * ctx = ts_cli_ctx_open();

    if(argc <= 1) return ts_cli_help(ctx, argc, argv);

    int (*op)(ts_cli_ctx *, int, char**) = 
        ts_args_matches("list", argv[1])        ? ts_cli_list:
        ts_args_matches("make", argv[1])        ? ts_cli_make:
        ts_args_matches("remove", argv[1])      ? ts_cli_remove:
        ts_args_matches("tag", argv[1])         ? ts_cli_tag:
        ts_args_matches("changeset", argv[1])   ? ts_cli_changeset:
        ts_args_matches("cs", argv[1])          ? ts_cli_changeset:
        ts_args_matches("presentset", argv[1])  ? ts_cli_presentset:
        ts_args_matches("pws", argv[1])         ? ts_cli_presentset:
        ts_args_matches("help", argv[1])        ? ts_cli_help:
        ts_args_matches("-help", argv[1])       ? ts_cli_help:
        ts_args_matches("--help", argv[1])      ? ts_cli_help:
                                                  ts_cli_help;
    

    int out = op(ctx, argc - 2, &argv[2]);

    ts_cli_ctx_close(ctx);

    return out;
}


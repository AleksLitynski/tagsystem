#include "tsclihelp.h"


int ts_cli_help(ts_cli_ctx * ctx, int argc, char * argv[]) {

    fprintf(ctx->out, 
        "General help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}


int ts_cli_list_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "ls help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

int ts_cli_make_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "mk help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

int ts_cli_remove_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "rm help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

int ts_cli_tag_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "tag help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

int ts_cli_changeset_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "cs help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

int ts_cli_presentset_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 
        "pws help  \n"
        "On many lines \n"
    );

    return TS_SUCCESS;
}

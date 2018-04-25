#include "tsclihelp.h"


int ts_cli_help(ts_cli_ctx * ctx, int argc, char * argv[]) {

    fprintf(ctx->out, 

"The tagsystem allows you to create, tag, and search for files.     \n"
"                                                                   \n"
"Subcommands:                                                       \n"
" list [--id] [--tags] [--help]                                     \n"
" make [--id] [--preview] [--silent] [--help] <tagset>              \n"
" remove [--id] [--force] [--silent] [--help] <tagset>              \n"
" tag [--id] [--force] [--silent] [--help] <tagset>                 \n"
" changeset|cs [--silent] [--help] <tagset>                         \n"
" presentset|pws [--help]                                           \n"
" help                                                              \n"
"                                                                   \n"
"Tagset syntax:                                                     \n"
" +animals      add the tag 'animals'                               \n"
" animals       add the tag 'animals'                               \n"
" ~animals      remove the tag 'animals'                            \n"
" +thrush~cat   add the tag 'busses', remove the tag 'cars'         \n"
" ~~            remove all tags                                     \n"
" ~~moles       remove all tags, then add the tag moles             \n"
"                                                                   \n"
"Subcommand Abbrivation:                                            \n"
" Subcommands can be abbriviated and vowels can be omitted.         \n"
" 'list', 'li', and 'ls' are all equivalent.                        \n"
"                                                                   \n"
"Flag Abbriviation:                                                 \n"
" Flags can also be shortened. Flags can be prefixed with either    \n"
" '--' meaning the full flag is expected, or shortened to '-'       \n"
" followed by a single letter. If multiple letters follow '-',      \n"
" each letter is split into it's own flag.                          \n"
"                                                                   \n"
"Samples:                                                           \n"
" tsys mk +bat  # make a new file tagged bat                        \n"
" tsys cs +bat  # change to the bat set                             \n"
" tsys ls       # print out all files in the bat set                \n"
" # apply the 'blind' tag to all files in the 'bat set              \n"
" tsys ls | tsys tag +blind                                         \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}


int ts_cli_list_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"List all files in the present working set                          \n"
"                                                                   \n"
"Flags:                                                             \n"
" --id      Print the file ids instead of the file names            \n"
" --tags    Print the file tags alongside the file names            \n"
" --help    Print this help text                                    \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

int ts_cli_make_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"Creates a new file with all current tags applied to it.            \n"
"                                                                   \n"
"If the pws is '+animal' and 'mk +cat' is run,                      \n"
"a new file will be created with the tags '+animal+cat'.            \n"
"                                                                   \n"
"Make will update the pws to include the new tags.                  \n"
"                                                                   \n"
"Flags:                                                             \n"
" --id          Print the new file's id instead of its file name    \n"
" --preview     Do not update the pws after creating the file       \n"
" --silent      Do not print the file name of the new file          \n"
" --help        Print this help text                                \n"
" <tagset>      The set of tags to apply to the file                \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

int ts_cli_remove_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"Deletes all files in the pws.                                      \n"
"                                                                   \n"
"If multiple files would be removed, prompts to confirm deletion.   \n"
"                                                                   \n"
"Flags:                                                             \n"
" --id      Print the ids of deleted files instead of the file names\n"
" --force   Do not prompt on deleting multiple files                \n"
" --silent  Do not print the file name of deleted files             \n"
" --help    Print this help text                                    \n"
" <tagset>  The set of tags to be deleted                           \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

int ts_cli_tag_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"Adds new tags to a set of files.                                   \n"
"                                                                   \n"
"Tags to be added or removed are provided as an argument. The files \n"
"to be tagged should be provided one file per line to stdin.        \n"
"                                                                   \n"
"For example: 'tsys ls | tsys tag +whale'                           \n"
"                                                                   \n"
"If multiple files would be tagged, prompts to confirm tagging.     \n"
"                                                                   \n"
"Flags:                                                             \n"
" --id      Print the ids of tagged files instead of the file names \n"
" --force   Do not prompt before tagging multiple files             \n"
" --silent  Do not print the file name of tagged files              \n"
" --help    Print this help text                                    \n"
" <tagset>  The set of tags to be applied to input files            \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

int ts_cli_changeset_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"Change the present working set.                                    \n"
"                                                                   \n"
"Most operations in the tagsystem are applied relative to a         \n"
"present working set (pws). For instance, new files are created     \n"
"at the insersection of the present working set and any new tags    \n"
"being applied to the new file.                                     \n"
"                                                                   \n"
"Flags:                                                             \n"
" --silent  Do not print the updated pws                            \n"
" --help    Print this help text                                    \n"
" <tagset>  The update to be applied to the pws                     \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

int ts_cli_presentset_help(ts_cli_ctx * ctx) {
    fprintf(ctx->out, 

"Print the present working set.                                     \n"
"                                                                   \n"
"Most operations in the tagsystem are applied relative to a         \n"
"present working set (pws). For instance, new files are created     \n"
"at the insersection of the present working set and any new tags    \n"
"being applied to the new file.                                     \n"
"                                                                   \n"
"Flags:                                                             \n"
" --help    Print this help text                                    \n"
"                                                                   \n"

    );

    return TS_SUCCESS;
}

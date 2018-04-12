#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "hash.h"
#include "sds.h"
#include "tserror.h"
#include "tscli.h"
#include "tsstr.h"
#include "tsargs.h"
#include "tstagset.h"
#include "tssearchset.h"
#include "tstaglist.h"


int ts_cli_list(ts_cli_ctx * ctx, int argc, char * argv[]) {

    ts_args args;
    ts_args_create(&args, 1);
    bool ** show_id = ts_args_add_bool(&args, "id");
    ts_args_parse(&args, argc, argv);


    ts_tagset_create(ctx->pws, args.rest);
    ts_search * search = ts_searchset_create(ctx);
    
    ts_id id;
    while(ts_search_next(search, &id) != TS_SEARCH_DONE) {
        ts_cli_print_id(ctx, &id, **show_id);
    }


    ts_searchset_close(search);
    ts_args_close(&args);
    return EXIT_SUCCESS;
}

int ts_cli_make(ts_cli_ctx * ctx, int argc, char * argv[]) {
    ts_args args;
    ts_args_create(&args, 3);
    bool ** show_id = ts_args_add_bool(&args, "id"); //(show created document's id or path)
    bool ** dont_change_pws = ts_args_add_bool(&args, "preview"); // (change directory on creation, or just create)
    bool ** dont_show_doc_name = ts_args_add_bool(&args, "silent"); // (don't display document name after create)
    ts_args_parse(&args, argc, argv);
    
    ts_tagset_create(ctx->pws, args.rest);

    ts_doc doc;
    ts_doc_create(&doc, ctx->db);

    hash_each(ctx->pws, {
        ts_doc_tag(&doc, key);
    })

    if(!**dont_show_doc_name) {
        ts_cli_print_id(ctx, &doc.id, **show_id);
    }

    if(!**dont_change_pws) {
        ts_cli_save_ctx(ctx);
    }


    ts_args_close(&args);
    return EXIT_SUCCESS;
}


int ts_cli_remove(ts_cli_ctx * ctx, int argc, char * argv[]) {

    ts_args args;
    ts_args_create(&args, 3);
    bool ** show_id = ts_args_add_bool(&args, "id"); // (show created document's id or path)
    bool ** force = ts_args_add_bool(&args, "force");// if deleting more than 1, confirm. -force to not confirm
    bool ** silent = ts_args_add_bool(&args, "silent"); // silent (don't display document names after deletion)
    ts_args_parse(&args, argc, argv);

    if(!ts_searchset_has_one(ctx) && !**force) {
        // prompt before deleting
        if(!ts_cli_confirm("Multiple documents will be deleted. Continue?")) {
            ts_args_close(&args);
            return EXIT_SUCCESS;
        }
    }

    ts_search * search = ts_searchset_create(ctx);
    bool has_one = true;
    ts_id id;
    while(ts_search_next(search, &id) != TS_SEARCH_DONE) {
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, id);
        ts_doc_delete(&doc);
        ts_doc_close(&doc);
        if(!**silent) {
            ts_cli_print_id(ctx, &doc.id, **show_id);
        }
    }

    ts_search_close(search);
    ts_args_close(&args);
    return EXIT_SUCCESS;
}


int ts_cli_tag(ts_cli_ctx * ctx, int argc, char * argv[]) {
    ts_args args;
    ts_args_create(&args, 1);
    bool ** show_id = ts_args_add_bool(&args, "id"); // (show created document's id or path)
    bool ** force = ts_args_add_bool(&args, "force"); // (if we should prompt before tagging multiple documents)
    ts_args_parse(&args, argc, argv);

    // __rest__ will be the tags to apply
    // stdin is the docs to apply tag to. May be either path or id format
    int count = 0;
    sds * paths = ts_cli_stdin_to_array(&count);

    if(count > 1 && !**force) {
        if(!ts_cli_confirm("Multiple documents will be tagged. Continue?")) {
            sdsfreesplitres(paths, count);
            ts_args_close(&args);
            return EXIT_SUCCESS;
        }
    }

    ts_id * ids = malloc(sizeof(ts_id) * count);
    ts_doc * docs = malloc(sizeof(ts_doc) * count);

    ts_taglist * tags = ts_taglist_create(args.rest);
    
    for(int i = 0; i < count; i++) {
        char * id_str = ts_cli_doc_path_id(paths[i]);
        ts_id id;
        ts_id_from_string(&id, id_str);
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, id);
        // apply all tags

        ts_taglist * current_tag = tags;

        while(current_tag != 0) {

            switch(current_tag->operation) {
                case TS_TAGLIST_ADD_TAG: {
                    ts_doc_tag(&doc, current_tag->name);
                    break;   
                }

                case TS_TAGLIST_DEL_TAG: {
                    ts_doc_untag(&doc, current_tag->name);
                    break;
                }
            }

            current_tag = current_tag->next;
        }
        
        ts_doc_close(&doc);
        free(id_str);
    }

    
    ts_taglist_close(tags);
    free(ids);
    free(docs);
    sdsfreesplitres(paths, count);
    ts_args_close(&args);
    return EXIT_SUCCESS;
}

int ts_cli_changeset(ts_cli_ctx * ctx, int argc, char * argv[]) {
    ts_args args;
    ts_args_create(&args, 1);
    bool ** silent = ts_args_add_bool(&args, "silent"); // silent (don't display new set after cws)
    ts_args_parse(&args, argc, argv);

    ts_tagset_create(ctx->pws, args.rest);
    ts_cli_save_ctx(ctx);
    if(!**silent) {
        sds pws = ts_cli_print_pws(ctx);
        printf("%s\n", pws);
        sdsfree(pws);
    }

    ts_args_close(&args);
    return EXIT_SUCCESS;
}

int ts_cli_presentset(ts_cli_ctx * ctx, int argc, char * argv[]) {

    // prints the pws. No args
    sds pws = ts_cli_print_pws(ctx);
    printf("%s\n", pws);
    sdsfree(pws);

    return EXIT_SUCCESS;
}

int ts_cli_help(ts_cli_ctx * ctx, int argc, char * argv[]) {
    printf("Do it right next time\n");
    return EXIT_SUCCESS;
}

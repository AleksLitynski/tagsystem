#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "main.h"
#include "parsing.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include "utils.h"

int main(int argc, char * argv[]) {

    cli_ctx ctx = {
        .db = get_db(),
        .pws = get_working_set()
    };

    if(argc <= 1) return help_cmd(&ctx, argc, argv);

    int (*op)(cli_ctx *, int, char**) = 
        matches("list", argv[1])        ? list_cmd:
        matches("make", argv[1])        ? make_cmd:
        matches("remove", argv[1])      ? remove_cmd:
        matches("tag", argv[1])         ? tag_cmd:
        matches("changeset", argv[1])   ? changeset_cmd:
        matches("cs", argv[1])          ? changeset_cmd:
        matches("presentset", argv[1])  ? presentset_cmd:
        matches("pws", argv[1])         ? presentset_cmd:
        matches("help", argv[1])        ? help_cmd:
        matches("-help", argv[1])       ? help_cmd:
        matches("--help", argv[1])      ? help_cmd:
                                          help_cmd;
    


    int out = op(&ctx, argc - 2, &argv[2]);

    tag_set_free(ctx.pws);
    ts_db_close(ctx.db);

    return out;
}


int list_cmd(cli_ctx * ctx, int argc, char * argv[]) {

    arg_list args;
    args_create(&args, 1);
    bool ** show_id = args_add_bool(&args, "id");
    args_parse(&args, argc, argv);


    tag_set(ctx->pws, args.rest);
    ts_search * search = search_set(ctx->pws, ctx->db);
    
    ts_id id;
    while(ts_search_next(search, &id) != TS_SEARCH_DONE) {
        print_id(&id, ctx->db, **show_id);
    }


    search_set_close(search);
    args_close(&args);
    return EXIT_SUCCESS;
}

int make_cmd(cli_ctx * ctx, int argc, char * argv[]) {
    arg_list args;
    args_create(&args, 3);
    bool ** show_id = args_add_bool(&args, "id"); //(show created document's id or path)
    bool ** dont_change_pws = args_add_bool(&args, "preview"); // (change directory on creation, or just create)
    bool ** dont_show_doc_name = args_add_bool(&args, "silent"); // (don't display document name after create)
    args_parse(&args, argc, argv);
    
    tag_set(ctx->pws, args.rest);

    ts_doc doc;
    ts_doc_create(&doc, ctx->db);

    hash_each(ctx->pws, {
        ts_doc_tag(&doc, key);
    })

    if(!**dont_show_doc_name) {
        print_id(&doc.id, ctx->db, **show_id);
    }

    if(!**dont_change_pws) {
        save_pws(ctx->pws);
    }


    args_close(&args);
    return EXIT_SUCCESS;
}


int remove_cmd(cli_ctx * ctx, int argc, char * argv[]) {

    arg_list args;
    args_create(&args, 3);
    bool ** show_id = args_add_bool(&args, "id"); // (show created document's id or path)
    bool ** force = args_add_bool(&args, "force");// if deleting more than 1, confirm. -force to not confirm
    bool ** silent = args_add_bool(&args, "silent"); // silent (don't display document names after deletion)
    args_parse(&args, argc, argv);

    if(!set_has_one_item(ctx) && !**force) {
        // prompt before deleting
        if(!confirm("Multiple documents will be deleted. Continue?")) {
            args_close(&args);
            return EXIT_SUCCESS;
        }
    }

    ts_search * search = search_set(ctx);
    bool has_one = true;
    ts_id id;
    while(ts_search_next(search, &id) != TS_SEARCH_DONE) {
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, id);
        ts_doc_delete(&doc);
        ts_doc_close(&doc);
        if(!**silent) {
            print_id(&doc.id, ctx->db, **show_id);
        }
    }

    search_set_close(search);
    args_close(&args);
    return EXIT_SUCCESS;
}


int tag_cmd(cli_ctx * ctx, int argc, char * argv[]) {
    arg_list args;
    args_create(&args, 1);
    bool ** show_id = args_add_bool(&args, "id"); // (show created document's id or path)
    bool ** force = args_add_bool(&args, "force"); // (if we should prompt before tagging multiple documents)
    args_parse(&args, argc, argv);

    // __rest__ will be the tags to apply
    // stdin is the docs to apply tag to. May be either path or id format
    int count = 0;
    sds * paths = stdin_to_array(&count);

    if(count > 1 && !**force) {
        if(!confirm("Multiple documents will be tagged. Continue?")) {
            sdsfreesplitres(paths, count);
            args_close(&args);
            return EXIT_SUCCESS;
        }
    }

    ts_id * ids = malloc(sizeof(ts_id) * count);
    ts_doc * docs = malloc(sizeof(ts_doc) * count);

    tag_list_item * tags = tag_list(args.rest);
    
    for(int i = 0; i < count; i++) {
        char * id_str = doc_path_id(paths[i]);
        ts_id id;
        ts_id_from_string(&id, id_str);
        ts_doc doc;
        ts_doc_open(&doc, ctx->db, id);
        // apply all tags

        tag_list_item * current_tag = tags;

        while(current_tag != 0) {

            switch(current_tag->operation) {
                case TS_CTX_ADD_TAG: {
                    ts_doc_tag(&doc, current_tag->name);
                    break;   
                }

                case TS_CTX_DEL_TAG: {
                    ts_doc_untag(&doc, current_tag->name);
                    break;
                }
            }

            current_tag = current_tag->next;
        }
        
        ts_doc_close(&doc);
        free(id_str);
    }

    
    tag_list_free(tags);
    free(ids);
    free(docs);
    sdsfreesplitres(paths, count);
    args_close(&args);
    return EXIT_SUCCESS;
}

int changeset_cmd(cli_ctx * ctx, int argc, char * argv[]) {
    arg_list args;
    args_create(&args, 1);
    bool ** silent = args_add_bool(&args, "silent"); // silent (don't display new set after cws)
    args_parse(&args, argc, argv);

    tag_set(ctx->pws, args.rest);
    save_pws(ctx->pws);
    if(!**silent) {
        sds pws = print_pws(ctx);
        printf("%s\n", pws);
        sdsfree(pws);
    }

    args_close(&args);
    return EXIT_SUCCESS;
}

int presentset_cmd(cli_ctx * ctx, int argc, char * argv[]) {

    // prints the pws. No args
    sds pws = print_pws(ctx);
    printf("%s\n", pws);
    sdsfree(pws);

    return EXIT_SUCCESS;
}

int help_cmd(cli_ctx * ctx, int argc, char * argv[]) {
    printf("Do it right next time\n");
    return EXIT_SUCCESS;
}

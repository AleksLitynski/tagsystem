
/*
    Core commands -
        change tag system (env var)
        change set
        get present working set
        list
            - show ids or path
        make
            - show ids or path
        remove
            - confirm delete many
        tag
        untag

    Proj commands -
        proj
            - force to overwrite directory
            - don't map names to name: and type: tags
        sync
            - pull first or push first
            - all file deletion
*/


/*
    env vars:
        - current db dir
        - current working set


    
    functions:
        - parse tag list
        - update tag list

*/

sds _ts_ctx_push_tag(hash_t * tags, sds next_tag, char mode) {
    if(sdslen(next_tag) != 0) {
        switch(mode) {
            case '+': hash_set(tags, next_tag, "");
            case '-': hash_del(tags, next_tag);
        }
        sdsfree(next_tag);
        next_tag = sdsempty();
    }

    return next_tag;
}

int ts_ctx_tag_list(sds tag_str, hash_t * tags) {

    int tag_str_len = strlen(tag_str);

    char mode = '+';
    sds next_tag = sdsempty();

    for(int i = 0; i < tag_str_len; i++) {
        char next = tag_str[i];

        if(isspace(next)) {
            // add current item if item has any value
            next_tag = _ts_ctx_push_tag(tags, next_tag, mode);
        }

        else if(next == '-') {
            // if the previous character is '-' (just check at i-1), remove all items from hash
            if(i > 0 && tag_str[i-1] == '-') {
                hash_clear(tags);
                mode = '+';
            }

            else {
                next_tag = _ts_ctx_push_tag(tags, next_tag, mode);
                mode = '-';
            }

        }

        else if(next == '+') {
            // add current item and set to '+' for next item
            next_tag = _ts_ctx_push_tag(tags, next_tag, mode);
            mode = '+';
        }

        else {
            // append next character to next_tag
            next_tag = sdscatlen(next_tag, &next_tag, 1);
        }
    }

    return TS_SUCCESS;
}


int ts_ctx_parse_args(int argc, char * argv[]) {
    
    return TS_SUCCESS;
}
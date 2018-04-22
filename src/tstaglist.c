#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "tstaglist.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

ts_taglist * _ts_taglist_next(sds * next_tag, char mode) {
    ts_taglist * item = malloc(sizeof(ts_taglist));
    item->next = 0;
    item->operation = 0;
    sdstrim(*next_tag, " ");
    if(sdslen(*next_tag) == 0) return 0;

    item->name = malloc(strlen(*next_tag));
    strcpy(item->name, *next_tag);

    item->operation = mode;
    
    sdsfree(*next_tag);
    *next_tag = sdsempty();

    return item;
}

ts_taglist * ts_taglist_create(sds tag_str) {
    ts_taglist * head = malloc(sizeof(ts_taglist));
    head->operation = ' ';
    head->next = 0;
    ts_taglist * next = head;


    int tag_str_len = strlen(tag_str);

    char mode = TS_TAGLIST_ADD_TAG;
    sds next_tag = sdsempty();

    for(int i = 0; i < tag_str_len; i++) {
        char next_char = tag_str[i];

        if(isspace(next_char)) {
            // add current item if item has any value
            if(strlen(next_tag) != 0) {
                next_tag = sdscat(next_tag, " ");
            }
        }

        else if(next_char == TS_TAGLIST_DEL_TAG) {
            // if the previous character is '-' (just check at i-1), remove all items from hash
            if(i > 0 && tag_str[i-1] == TS_TAGLIST_DEL_TAG) {
                ts_taglist_close(head);
                head = malloc(sizeof(ts_taglist));
                head->operation = ' ';
                head->next = 0;
                next = head;
                
                mode = TS_TAGLIST_ADD_TAG;
            }

            else {
                ts_taglist * new_item = _ts_taglist_next(&next_tag, mode);
                if(new_item != 0) {
                    next->next = new_item;
                    next = new_item;
                }

                mode = TS_TAGLIST_DEL_TAG;
            }

        }

        else if(next_char == TS_TAGLIST_ADD_TAG) {
            // add current item and set to '+' for next item
            ts_taglist * new_item = _ts_taglist_next(&next_tag, mode);
            if(new_item != 0) {
                next->next = new_item;
                next = new_item;
            }

            mode = TS_TAGLIST_ADD_TAG;
        }

        else {
            // append next character to next_tag
            next_tag = sdscatlen(next_tag, &next_char, 1);
        }
    }

    next->next = _ts_taglist_next(&next_tag, mode);
    return head;

}

int ts_taglist_close(ts_taglist * head) {
    if(head->next != 0) ts_taglist_close(head->next);
    if(head->operation != ' ') free(head->name);
    free(head);
    return TS_SUCCESS;
}

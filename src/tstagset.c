
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "hash.h"
#include "sds.h"
#include "tstaglist.h"
#include "tstagset.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"

int ts_tagset_create(hash_t * tags, sds tag_str) {
    ts_taglist * head = ts_taglist_create(tag_str);
    ts_taglist * current = head;

    do {

        switch(current->operation) {
            case TS_TAGLIST_ADD_TAG: {
                char * key = malloc(strlen(current->name));
                strcpy(key, current->name);

                hash_set(tags, key, key); 
                break;
            }

            case TS_TAGLIST_DEL_TAG: {
                char * val = hash_get(tags, current->name);
                if(val != NULL) {
                    hash_del(tags, current->name); 
                    free(val);
                }

                break;
            }
        }

        current = current->next;

    } while(current != 0);


    ts_taglist_close(head);
    return TS_SUCCESS;
}

int ts_tagset_close(hash_t * tags) {
    hash_each(tags, {
        free(val);
    })

    hash_free(tags);
    return TS_SUCCESS;
}

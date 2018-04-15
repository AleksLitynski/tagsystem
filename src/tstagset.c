
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

int ts_tagset_append(hash_t * tags, sds tag_str) {
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

hash_t * ts_tagset_load() {
    hash_t * t = hash_new();
    char * pws_str = getenv("TSPWS");
    if(pws_str != 0) ts_tagset_append(t, pws_str);
    return t;
}

void ts_tagset_save(hash_t * tags) {
    sds pws_str = ts_tagset_print(tags);
    setenv("TSPWS", pws_str, true);
    sdsfree(pws_str);
}

sds ts_tagset_print(hash_t * tags) {
    sds pws_str = sdsempty();
    hash_each(tags, {
        pws_str = sdscat(pws_str, "+");
        pws_str = sdscat(pws_str, key);
    })
    return pws_str;
}

int ts_tagset_close(hash_t * tags) {
    hash_each(tags, {
        free(val);
    })

    hash_free(tags);
    return TS_SUCCESS;
}

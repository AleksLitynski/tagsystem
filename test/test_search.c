#include "test.h"


void search_test(void ** state) {
    test_state * st = (test_state*)*state;

    int id_count = 20;
    ts_id ids[id_count];
    for(int i = 0; i < id_count; i++) {
        // ts_id_empty(ids + i);
        ts_id_generate(ids + i, st->db);
    }
    // id_from_binary_string("01", &ids[0]);
    // id_from_binary_string("10", &ids[1]);

    ts_tags tags;
    ts_tags_empty(&tags);
    for(int i = 0; i < id_count; i++) {
        LOGIDBIN(ids + i);
        ts_tags_insert(&tags, ids + i);
    }


    ts_search search;
    ts_search_create(&search, &tags, 1);

    ts_id found_ids[id_count];
    int id_idx = 0;
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        // LOGID(found_ids + id_idx);
        id_idx++;
    }

    LOG1("Missing:");
    for(int i = 0; i < id_count; i++) {
        bool missing = true;
        ts_id * next = ids + i;
        for(int j = 0; j < id_count; j++) {
            if(ts_id_eq(next, found_ids + j)) {
                missing = false;
            }
        }

        if(missing) {
            LOG1("Missing:");
            LOGIDBIN(next);
        } else {
            // LOG1("Not missing:");
            // LOGID(next);
        }
    }

    ts_search_close(&search);
    ts_tags_close(&tags);
    
}


// int ts_search_create(ts_search * self, ts_db * db, ts_tags * tags, int tag_count);
// int ts_search_step(ts_search * self, ts_id * id); 
// int ts_search_next(ts_search * self, ts_id * id); 
// int _ts_search_tag_close(ts_search_tag * tags);
// int ts_search_close(ts_search * self);

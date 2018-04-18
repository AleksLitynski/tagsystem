#include "test.h"


void search_test(void ** state) {
    test_state * st = (test_state*)*state;

    int id_count = 100;
    ts_id ids[id_count];
    for(int i = 0; i < id_count; i++) {
        ts_id_generate(ids + i, st->db);
    }

    ts_tags tags;
    ts_tags_empty(&tags);
    for(int i = 0; i < id_count; i++) {
        ts_tags_insert(&tags, ids + i);
    }


    ts_search search;
    ts_search_create(&search, &tags, 1);

    ts_id found_ids[id_count];
    int id_idx = 0;
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        id_idx++;
    }

    bool missing = true;
    for(int i = 0; i < id_count; i++) {
        ts_id * next = ids + i;
        for(int j = 0; j < id_count; j++) {
            if(ts_id_eq(next, found_ids + j)) {
                missing = false;
            }
        }

        if(missing) {
            LOG1("Missing:");
            LOGIDBIN(next);
        }
    }

    ts_search_close(&search);
    ts_tags_close(&tags);


    assert_false(missing);
    
}



void search_intersection_test(void ** state) {
    test_state * st = (test_state*)*state;

    int id_count = 1;

    ID(10001011);
    ID(10001110);
    ID(00101010);

    ts_tags tags[2];
    ts_tags_empty(&tags[0]);
    // for(int i = 0; i < id_count; i++) {
    //     ts_id id;
    //     ts_id_generate(&id, st->db);
    //     ts_tags_insert(&tags[0], &id);
    // }

    ts_tags_empty(&tags[1]);
    // for(int i = 0; i < id_count; i++) {
    //     ts_id id;
    //     ts_id_generate(&id, st->db);
    //     ts_tags_insert(&tags[1], &id);
    // }

    LOGTAGS(&tags[0]);
    ts_tags_insert(&tags[0], &id_10001011);
    LOGTAGS(&tags[0]);
    ts_tags_insert(&tags[0], &id_10001110);
    LOGTAGS(&tags[0]);
    ts_tags_insert(&tags[1], &id_00101010);
    ts_tags_insert(&tags[1], &id_10001110);

    // ts_id common_id;
    // ts_id_generate(&common_id, st->db);
    // ts_tags_insert(&tags[0], &common_id);
    // ts_tags_insert(&tags[1], &common_id);
    LOG1("Tags 0:");
    LOGTAGS(&tags[0]);
    LOG1("Tags 1:");
    LOGTAGS(&tags[1]);


    ts_search search;
    ts_search_create(&search, tags, 2);

    ts_id found_ids[id_count];
    int id_idx = 0;
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        id_idx++;
    }


    ts_search_close(&search);
    ts_tags_close(&tags[0]);
    ts_tags_close(&tags[1]);

    LOG("id_idx:%i", id_idx);
    LOGIDBIN(&found_ids[0]);
    // assert_true(ts_id_eq(&found_ids[0], &common_id));
    assert_int_equal(id_idx, 1);
    
}


#include "test.h"


void search_test(void ** state) {
    test_state * st = (test_state*)*state;

    //a generate 256 ids
    int id_count = 256;
    ts_id ids[id_count];
    for(int i = 0; i < id_count; i++) {
        ts_id_empty(ids + i);
        ids[i][0] = i;
    }

    // add all ids to a tree
    ts_tags tags;
    ts_tags_empty(&tags);
    for(int i = 0; i < id_count; i++) {
        ts_tags_insert(&tags, ids + i);
    }

    ts_search search;
    ts_search_create(&search, &tags, 1);
    
    // get each id from the tree
    int id_idx = 0;
    ts_id found_ids[id_count];
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        id_idx++;
    }

    // confirm each id was found
    int missing_any = false;
    for(int i = 0; i < id_count; i++) {
        bool missing = true;
        ts_id * next = ids + i;
        for(int j = 0; j < id_idx; j++) {
            if(ts_id_eq(next, found_ids + j)) {
                missing = false;
            }
        }

        if(missing) {
            LOG1("Missing:");
            LOGIDBIN(next);
            missing_any = true;
        }
    }

    ts_search_close(&search);
    ts_tags_close(&tags);


    assert_false(missing_any);
    
}

void search_remove_test(void ** state) {

    // same as 'search_test', but removes one id and confirms it is removed
    test_state * st = (test_state*)*state;

    int id_count = 256;
    ts_id ids[id_count];
    for(int i = 0; i < id_count; i++) {
        ts_id_empty(ids + i);
        ids[i][0] = i;
    }

    ts_tags tags;
    ts_tags_empty(&tags);
    for(int i = 0; i < id_count; i++) {
        ts_tags_insert(&tags, ids + i);
    }

    ts_tags_remove(&tags, &ids[id_count - 1]);

    ts_search search;
    ts_search_create(&search, &tags, 1);
    

    int id_idx = 0;
    ts_id found_ids[id_count];
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        id_idx++;
    }

    ts_search_close(&search);
    ts_tags_close(&tags);


    assert_int_equal(id_idx, id_count - 1);
    
}


void search_intersection_test(void ** state) {
    test_state * st = (test_state*)*state;

    int id_count = 20;

    // insert 20 ids in each set
    // insert another 20 into both sets
    ts_tags tags[2];
    ts_tags_empty(&tags[0]);
    ts_tags_empty(&tags[1]);

    for(int i = 0; i < id_count; i++) {
        ts_doc doc;
        ts_doc_create(&doc, st->db);    // 1/3 go in set 0
        ts_tags_insert(&tags[0], &doc.id);
        ts_doc_close(&doc);

        ts_doc_create(&doc, st->db);    // 1/3 go in set 1
        ts_tags_insert(&tags[1], &doc.id);
        ts_doc_close(&doc);
        
        ts_doc_create(&doc, st->db);    // 1/3 of tags go in set 0 and 1
        ts_tags_insert(&tags[0], &doc.id);
        ts_tags_insert(&tags[1], &doc.id);

        ts_doc_close(&doc);
    }

    ts_search search;
    ts_search_create(&search, tags, 2);

    ts_id found_ids[id_count];
    int id_idx = 0;
    ts_tags common_tags;
    ts_tags_empty(&common_tags);
    while(ts_search_next(&search, found_ids + id_idx) != TS_SEARCH_DONE) {
        ts_tags_insert(&common_tags, found_ids + id_idx);
        id_idx++;
    }

    ts_search_close(&search);
    ts_tags_close(&tags[0]);
    ts_tags_close(&tags[1]);

    assert_int_equal(id_idx, id_count);

    
}


#include "test.h"


void tags_test(void ** state) {
    test_state * st = (test_state*)*state;
    
    // create an empty tag tree
    clock_t begin = clock();
    ts_tags tags;
    ts_tags_empty(&tags);
    
    ts_doc doc;
    for(int i = 0; i < 10; i++) {
        ts_doc_create(&doc, st->db);
        ts_tags_insert(&tags, &doc.id);
    }                       
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 

    // Shouldn't take more than 64 nodes to fit 10 elements. This is a shitty test
    assert_in_range(tags.size, 0, 64); 

    ts_doc_close(&doc);
    ts_tags_close(&tags);
}

void tag_empty_test(void ** state) {
    test_state * st = (test_state*)*state;

    ts_tags tags;
    ts_tags_empty(&tags);
    assert_int_equal(tags.size, 2);
    assert_int_equal(tags.occupied, 1);
    assert_int_equal(tags.next, 1);
    assert_true(tags.data->type == TS_TAG_NODE_INNER);

}

void tag_insert_test(void ** state) {
    test_state * st = (test_state*)*state;

    ts_tags tags;
    ts_tags_empty(&tags);

    ID(0000);
    ID(0001);
    ID(0010);
    ID(0011);
    ID(0100);
    ID(0101);
    ID(0110);
    ID(0111);
    ts_tags_insert(&tags, &id_0000);
    assert_int_equal(tags.occupied, 2);
    ts_tags_insert(&tags, &id_0001);
    assert_int_equal(tags.occupied, 6);
    ts_tags_insert(&tags, &id_0010);
    assert_int_equal(tags.occupied, 7);
    ts_tags_insert(&tags, &id_0011);
    assert_int_equal(tags.occupied, 9);
    ts_tags_insert(&tags, &id_0100);
    assert_int_equal(tags.occupied, 10);
    ts_tags_insert(&tags, &id_0101);
    assert_int_equal(tags.occupied, 13);
    ts_tags_insert(&tags, &id_0110);
    assert_int_equal(tags.occupied, 14);
    ts_tags_insert(&tags, &id_0111);
    assert_int_equal(tags.occupied, 16);

    ts_tags_close(&tags);
    

}

void tag_remove_test(void ** state) {
    test_state * st = (test_state*)*state;

    ts_tags tags;
    ts_tags_empty(&tags);

    ID(0000);
    ID(0001);
    ID(0010);
    ID(0011);
    ID(0100);
    ID(0101);
    ID(0110);
    ID(0111);
    ts_tags_insert(&tags, &id_0000);
    ts_tags_insert(&tags, &id_0001);
    ts_tags_insert(&tags, &id_0010);
    ts_tags_insert(&tags, &id_0011);
    ts_tags_insert(&tags, &id_0100);
    ts_tags_insert(&tags, &id_0101);
    ts_tags_insert(&tags, &id_0110);
    ts_tags_insert(&tags, &id_0111);

    ts_tags_remove(&tags, &id_0110);

    // should be able to insert then remove some items without erroring out
    ts_tags_close(&tags);
    
}

void tag_shuffle_test(void ** state) {
    test_state * st = (test_state*)*state;

    int items = 10;

    ts_doc docs[items];
    ts_tags tags;
    ts_tags_empty(&tags);
    
    // insert x random ids
    for(int i = 0; i < items; i++) {
        ts_doc_create(&docs[i], st->db);
        ts_tags_insert(&tags, &docs[i].id);
    }

    // remove 1/3 of x random ids
    for(int i = 0; i < items / 3; i++) {
        ts_tags_remove(&tags, &docs[i].id);
    }
    
    // insert another x random ids
    for(int i = 0; i < items; i++) {
        ts_doc doc;
        ts_doc_create(&doc, st->db);
        ts_tags_insert(&tags, &doc.id);
    }

    for(int i = 0; i < items; i++) {
        ts_doc_close(&docs[i]);
    }

    // as long as none of these errored out, we're good
    ts_tags_close(&tags);
}

void tag_double_ops_test(void ** state) {
    test_state * st = (test_state*)*state;

    ts_tags tags;
    ts_tags_empty(&tags);

    ID(01);

    // shouldn't error when inserting twice
    ts_tags_insert(&tags, &id_01);
    ts_tags_insert(&tags, &id_01);

    // shouldn't error when removing twice
    ts_tags_remove(&tags, &id_01);
    ts_tags_remove(&tags, &id_01);

    ts_tags_close(&tags);
}

void tag_mdb_test(void ** state) {
    test_state * st = (test_state*)*state;

    // create a document
    ts_doc doc;
    ts_doc_create(&doc, st->db);

    // create a tag
    ts_tags tags;
    ts_tags_empty(&tags);
    ts_tags_insert(&tags, &doc.id);

    // write the tag
    ts_tags_write(&tags, st->db, "tag_name");

    // read the tag
    ts_tags tags_read;
    ts_tags_open(&tags_read, st->db, "tag_name");    

    // confirm the content
    ts_search search;
    ts_search_create(&search, &tags_read, 1);

    ts_id id;
    while(ts_search_next(&search, &id) != TS_SEARCH_DONE);

    assert_true(ts_id_eq(&doc.id, &id));

    ts_doc_close(&doc);
    ts_tags_close(&tags);
    ts_tags_close(&tags_read);
    ts_search_close(&search);

}
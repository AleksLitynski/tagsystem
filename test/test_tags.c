#include "test.h"


void tags_test(void ** state) {
    test_state * st = (test_state*)*state;
    
    clock_t begin = clock();
    ts_tags tags;
    ts_tags_empty(&tags);
    // ts_tags_empty(&tags);
    LOG1("Created empty tag tree");

    
    ts_id id;
    // 1mil inserts ~10s
    for(int i = 0; i < 10; i++) {
        ts_id_generate(&id, st->db);
        ts_tags_insert(&tags, &id);
    }                       
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 

    // LOGTAGS(&tags);
    LOG("Time to insert elems: %f", time_spent);
    LOG("size: %i", tags.size); 
    LOG("occupied: %i", tags.occupied);  
    // LOGTAGS(&tags); 
    assert_int_equal(tags.size, 32); // seems like we always end up w/ 32 size. Possibly we could exceed the block, but i'm not doing the math ;)
    
    ts_tags_remove(&tags, &id);
    LOG1("Removed id from tag tree");
    // LOGTAGS(&tags);

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


int ts_tags_remove(ts_tags * self, ts_id * id);
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

    ts_tags_remove(&tags, &id_0000);
    LOGTAGS(&tags);

    ts_tags_close(&tags);
    

}
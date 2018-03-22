#include "test.h"

void id_test(void ** state) {
    test_state * st = (test_state*)*state;

    int items = 20;
    ts_id ids[items];
    
    for(int i = 0; i < items; i++) {
      ts_id_generate(&ids[i], st->db);
    }
    
    int duplicates = 0;
    for(int i = 0; i < items; i++) {
        for(int j = 0; j < items; j++) {
            if(ts_id_eq(&ids[i], &ids[j]) && i != j) {
                duplicates++;
                LOG1("DUPLICATE:");
            }
        }

        LOGID(&ids[i]);
    }

    assert_int_equal(duplicates, 0);
}

void id_value_test(void ** state) {

    ts_id sample_1;
    set_id(&sample_1, 255);
    LOGID(&sample_1);
    for(int i = 0; i < TS_ID_BITS; i++) { 
        assert_int_equal(ts_id_get_bit(&sample_1, i), 1);
    }
    
    ts_id sample_0;
    set_id(&sample_0, 0);
    LOGID(&sample_0);
    for(int i = 0; i < TS_ID_BITS; i++) { 
        assert_int_equal(ts_id_get_bit(&sample_0, i), 0);
    }
    
    ts_id sample_1010;
    set_id(&sample_1010, 170);
    LOGID(&sample_1010);
    for(int i = 0; i < TS_ID_BITS; i++) {
        int v = ts_id_get_bit(&sample_1010, i);
        assert_int_equal(v, !(i % 2));
    }
}

void id_to_str_test(void ** state) {
    ts_id sample;
    set_id(&sample, 170);
    sds sample_str = ts_id_string(&sample, sdsempty());
    ts_id sample_from_str;
    ts_id_from_string(&sample_from_str, sample_str);

    for(int i = 0; i < TS_ID_BYTES; i++) {
        assert_int_equal(sample[i], sample_from_str[i]);
    }
    sdsfree(sample_str);
}

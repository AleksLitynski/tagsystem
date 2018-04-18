#include "test.h"

void id_value_test(void ** state) {

    // Should all be 1
    ts_id sample_1;
    set_id(&sample_1, 255);
    for(int i = 0; i < TS_ID_BITS; i++) { 
        assert_int_equal(ts_id_get_bit(&sample_1, i), 1);
    }
    
    // Should all be 0
    ts_id sample_0;
    set_id(&sample_0, 0);
    for(int i = 0; i < TS_ID_BITS; i++) { 
        assert_int_equal(ts_id_get_bit(&sample_0, i), 0);
    }
    
    // Should alternate 1 and 0
    ts_id sample_1010;
    set_id(&sample_1010, 170);
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

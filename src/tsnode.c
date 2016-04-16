#include "tsnode.h"


// val->mv_data must have TS_MAX_NODE_SIZE_BYTES free space 
void ts_node_to_mdb_val(
        ts_node * node, 
        int id_size_bits, int starting_offset_bits, 
        unsigned int new_jump, int new_jump_index,
        MDB_val * val) {
    
    int idSizeBytes = (id_size_bits+8-1)/8;
    int jumpCount = 0;
    for(int i = 0; i < idSizeBytes; i++) { 

        *((uint8_t *) (val->mv_data + i)) = 0; 
    }
    for(int i = 0; i < id_size_bits; i++){
        int j = i + starting_offset_bits;
        ((uint8_t *)val->mv_data)[i/8] |= node->doc_id[j/8]&(1<<j%8);

        uint8_t mask = node->mask[j/8]&(1<<j%8);
        if(new_jump && new_jump_index == i) mask = (1<<j%8);
        ((uint8_t *)val->mv_data)[id_size_bits + (i/8)] |= mask;

        if(mask) {
            unsigned int * jump = &((unsigned int *)val->mv_data)[idSizeBytes * 2];
            *jump += jumpCount * sizeof(unsigned int);
            if(new_jump && new_jump_index == i) {
                *jump = new_jump;
            } else {
                *jump = node->jumps[jumpCount];
            }
            jumpCount++;
        }
    }

    val->mv_size = (idSizeBytes * 2) + (jumpCount * sizeof(unsigned int));
}

// node will point into val, so don't let val be de-allocated
void ts_node_from_mdb_val(MDB_val * val, int id_size_bits, ts_node * node) {
    int idSizeBytes = (id_size_bits+8-1)/8;
    uint8_t * data = val->mv_data; 
    node->doc_id = data; 
    node->mask = &data[idSizeBytes];
    node->jumps = &((unsigned int *)data)[idSizeBytes * 2];
}


/*
struct { int pos; } ts_sample_data;

ts_sample_data ts_sample_init();
void ts_sample_next(ts_sample_data * data);
ts_sample_close(ts_sample_data * data);



ts_node_itr_data data_ctx;
ts_node_itr_item data;
ts_node_itr_create(&data_ctx, node);
while(ts_node_itr_next(&data_ctx, &data)) {
    // act on data 
}
ts_node_itr_close(&data);

itr(ts_node_itr, args(node), data, {

});


typedef struct {
    int idx;    
    ts_node * node;
}ts_node_itr_data;

typedef struct {
    uint8_t value;
} ts_node_itr_item;

void ts_node_itr_create(ts_node_itr_data * data, ts_node * node) {
    data->idx = 0;
    data->node = node;
}

void ts_node_itr_close(ts_node_itr_data * data) {

}

int ts_node_itr_next(ts_node_itr_data * data, ts_node_itr_item * item) {
    if(data->idx < data->node->size) {
        *item = ts_util_test_bit(data->node->doc_id_fragment, data->data->idx);
        data->idx++;
        return 1;
    } else {
        return 0;
    }
}
*/



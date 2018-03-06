#include "tsnode.h"
#include "tsdoc.h"
#include <stdio.h>

/*
typedef struct {
    int size;
    unsigned int key; // 0 means allocate a new one for me
    uint8_t * doc_id;
    uint8_t * mask;
    unsigned int * jumps;
} ts_node;
*/

// val->mv_data must have TS_MAX_NODE_SIZE_BYTES free space 
void ts_node_to_MDB_val(
        ts_node * node, 
        int id_size_bits, int starting_offset_bits, 
        unsigned int new_jump, int new_jump_index,
        MDB_val * val) {
   
    uint8_t * data = (uint8_t *)val->mv_data;
    int id_size = id_size_bits/8;
    int jumps = 0;
    for(int idx = 0; idx < id_size_bits; idx++){
        int idx_byte = idx/8;
        uint8_t idx_bit = 1 << (7 - (idx%8));
        int has_jump = new_jump && new_jump_index == idx;

        // printf("[%i - %i] ", idx_byte, idx_bit);

        uint8_t doc  = node->doc_id[idx_byte] & idx_bit;
        uint8_t mask = has_jump ? idx_bit : node->mask[idx_byte] & idx_bit;

        // printf("%i:%" PRIu8 " ", idx, doc);
        data[idx_byte]           |= doc;
        data[idx_byte + id_size] |= mask;

        if(mask) {
            uint8_t firstJump8 = data[id_size * 2 + 1];
            unsigned int * firstJump = (unsigned int *) &firstJump8;
            unsigned int * jump = firstJump + jumps;

            *jump = has_jump ? new_jump : node->jumps[jumps];
            jumps++;
        }
    }
    val->mv_size = (id_size * 2) + (jumps * sizeof(unsigned int));

    /*
    printf("id_size_bits: %i\n", id_size_bits);
    printf("starting_offset_bits: %i\n", starting_offset_bits);
    printf("new_jump: %i\n", new_jump);
    printf("new_jump_index: %i\n", new_jump_index);

    char * doc_str = ts_util_str_id((ts_doc_id *)node->doc_id);
    char * mask_str = ts_util_str_id((ts_doc_id *)node->mask);
    printf("node: \n");
    printf("    size: %i\n", node->size);
    printf("    key: %i\n", node->key);
    printf("    doc_id: %s\n", doc_str);
    printf("    mask:   %s\n", mask_str);
    free(doc_str);
    free(mask_str);
 
    printf("final:  ");
    for(int i = 0; i < val->mv_size; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
    printf("Jumps: %i\n", jumps);
    */
}

// node will point into val, so don't let val be de-allocated
void ts_node_from_MDB_val(MDB_val * val, int id_size_bits, ts_node * node) {
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



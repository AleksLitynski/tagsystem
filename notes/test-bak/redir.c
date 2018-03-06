#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>


#define TS_ID_BYTES 10
#define TS_ID_BITS 80
typedef uint8_t ts_doc_id[TS_ID_BYTES];

int bit_test(uint8_t * id, int loc) {
    return (id[loc/8] & (((uint8_t)1) << ( 8 - (loc % 8)))) > 0;
}


int main(int argc, char * argv[]) {
    ts_doc_id data;
    srand(time(NULL));
    for(int i = 0; i < TS_ID_BYTES; i++) data[i] = rand();

    ts_doc_id * id = &data;
    uint8_t * idVal = *id;



    for(int i = 0; i < TS_ID_BITS; i++) {
        int funcVal = bit_test(*id, i);

        int localVal = ((*id)[i/8] & (((uint8_t)1) << ( 8 - (i % 8)))) > 0;

        (localVal != funcVal) ?
            printf("_"):
            printf("%i", funcVal);
    }
    printf("\n");


    return 0;
}

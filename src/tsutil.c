#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "openssl/sha.h"


char * ts_util_concat(char * a, char * b) {
    int l1 = strlen(a);
    int l2 = strlen(b);
    char * c = malloc (l1 + l2 + 1);
    memcpy(c, a, l1);
    memcpy(c + l1, b, l2 + 1);
    return c;
}

void ts_util_safe_mk(char * name) {
    struct stat st = {0};
    if(stat(name, &st) == -1) {
        mkdir(name, 0700);
    }
}
uint8_t ts_util_test_bit(uint8_t * firstByte, unsigned int index) {
    unsigned int indexByte = index / 8;
    unsigned int indexBit  = index % 8;
    return firstByte[indexByte] & (1 << indexBit);
}



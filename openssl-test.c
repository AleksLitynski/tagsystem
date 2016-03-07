
#include <stdio.h>
#include "lib/openssl/include/openssl/sha.h"
#include <string.h>

int main(int argc, char * argv[]) {
    unsigned char obuf[20];
    unsigned char ibuf[] = "get my sha1";
    printf("%s => ", ibuf);

    SHA1(ibuf, strlen(ibuf), obuf);

    for(int i = 0; i < 20; i++) {
        printf("%02x", obuf[i]);
    }
    printf("\n");
    // output should be:
    // get my sha1 => 8940278896675ec1964555e4c766b66688d76504
    return 0;
}


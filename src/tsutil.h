#ifndef _TS_UTIL_H_
#define _TS_UTIL_H_

// includes
#include <stdint.h>

// macros
#define TS_KEY_SIZE_BITS 160
#define TS_KEY_SIZE_BYTES 20
#define TS_KEY_SIZE_BITS_DOUBLE 320
#define TS_KEY_SIZE_BYTES_DOUBLE 40
#define TS_MAX_NODE_SIZE_BITS (TS_KEY_SIZE_BITS_DOUBLE + (sizeof(unsigned int) * TS_KEY_SIZE_BITS))
#define TS_MAX_NODE_SIZE_BYTES (TS_KEY_SIZE_BYTES_DOUBLE + (sizeof(unsigned int) * TS_KEY_SIZE_BYTES))

// types

// functions
char  * ts_util_concat(char * a, char * b);
void    ts_util_safe_mk(char * name);
uint8_t ts_util_test_bit(uint8_t * firstByte, unsigned int index);

#endif


#ifndef _TS_UTIL_H_
#define _TS_UTIL_H_

// includes
#include <stdint.h>

// macros
#define TS_ID_BITS 160
#define TS_ID_BYTES 20
#define TS_NODE_BITS ((2*TS_ID_BITS) + (sizeof(unsigned int) * TS_ID_BITS))
#define TS_NODE_BYTES ((2*TS_ID_BYTES) + (sizeof(unsigned int) * TS_ID_BYTES))

// types

// functions
char  * ts_util_concat(char * a, char * b);
void    ts_util_safe_mk(char * name);
uint8_t ts_util_test_bit(uint8_t * firstByte, unsigned int index);

#endif


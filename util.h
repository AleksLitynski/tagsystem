#ifndef _ts_util_h_
#define _ts_util_h_

char * ts_util_concat(char * a, char * b);
void   ts_util_safe_mkdir(char * dirname);
uint8_t ts_util_test_bit(uint8_t * firstByte, unsigned int index);

#endif

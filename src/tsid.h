#pragma once

// includes
#include <stdint.h>
#include <stdbool.h>
#include "tsdb.h"

// macros
#define TS_ID_BYTES 20

// types
typedef uint8_t ts_id[TS_ID_BYTES];


// functions
int ts_id_generate(ts_id * self, ts_db * db);
int ts_id_string(ts_id * self, sds str);
bool ts_id_eq(ts_id * self, ts_id * other);

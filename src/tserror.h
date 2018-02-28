#pragma once

// includes

// macros
#define TS_ERROR_STRING(CODE) { \
    switch(CODE) { \
        case TS_SUCCESS:            return "Success"; \
        case TS_FAILURE:            return "Failure"; \
        case TS_KEY_NOT_FOUND:      return "LMDB Key not found"; \
    } \
}

// #define TS_SUCCESS 0
// #define TS_FAILURE 1
// #define TS_KEY_NOT_FOUND 3

// types
typedef enum {
    TS_SUCCESS,
    TS_FAILURE,
    TS_KEY_NOT_FOUND
} ts_error;

// functions
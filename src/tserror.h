#pragma once

// includes

// macros
#define TS_ERROR_STRING(CODE) { \
    switch(CODE) { \
        case TS_SUCCESS:            return "Success"; \
        case TS_FAILURE:            return "Failure"; \
        case TS_KEY_NOT_FOUND:      return "LMDB Key not found"; \
        case TS_TAGS_TREE_FULL:     return "Tag tree is full. use ts_tags_resize to grow the tree"; \
        case TS_SEARCH_DONE:        return "All items found in tag tree have been returned"; \
        case TS_SEARCH_NONE:        return "No items were found at this step through the tag tree, but search is not finished"; \
        case TS_SEARCH_FOUND:       return "An item was found searching the tag tree, but more items may be available"; \
    } \
}

// mostly chosing this format to match the unit tests :)
#define LOG(fmt, ...) printf ("[ INFO     ] " fmt "\n", __VA_ARGS__)
#define LOG1(fmt) LOG(fmt, "")

// types
typedef enum {
    TS_SUCCESS,
    TS_FAILURE,
    TS_KEY_NOT_FOUND,
    TS_TAGS_TREE_FULL,
    TS_SEARCH_DONE, 
    TS_SEARCH_FOUND, 
    TS_SEARCH_NONE
} ts_error;

// functions

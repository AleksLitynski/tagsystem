#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros

// types

// functions
// sds _ts_tagset_push(hash_t * tags, sds next_tag, char mode);
int ts_tagset_create(hash_t * tags, sds tag_str);
int ts_tagset_close(hash_t * tags);
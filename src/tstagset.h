#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros

// types

// functions

int ts_tagset_append(hash_t * tags, sds tag_str);
hash_t * ts_tagset_load();
void ts_tagset_save(hash_t * tags);
sds ts_tagset_print(hash_t * tags);
int ts_tagset_close(hash_t * tags);
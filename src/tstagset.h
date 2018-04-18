#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"
#include "tscliutils.h"

// macros

// types

// functions

int ts_tagset_append(hash_t * tags, sds tag_str);
hash_t * ts_tagset_load(ts_cli_ctx * ctx);
void ts_tagset_save(ts_cli_ctx * ctx, hash_t * tags);
sds ts_tagset_print(hash_t * tags);
int ts_tagset_close(hash_t * tags);


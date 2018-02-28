#pragma once

// includes
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include "../lib/sds/sds.h"
#include "tsid.h"
#include <stdint.h>

// macros

// types
typedef struct { 
    ts_id id;
    ts_db * env;
} ts_doc;


// functions
int ts_doc_create(ts_doc * self, ts_db * db);
int ts_doc_delete(ts_doc * self);

int ts_doc_open(ts_doc * self, ts_db * db, ts_id id);
int ts_doc_close(ts_doc * self);

int ts_doc_tag(ts_doc * self, sds tag);
int ts_doc_untag(ts_doc * self, sds tag);
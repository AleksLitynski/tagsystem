#ifndef _TS_DOC_H_
#define _TS_DOC_H_

// includes
#include "tsenv.h"
#include "lib/lmdb/libraries/liblmdb/lmdb.h"
#include <stdint.h>

// macros

// types
typedef uint8_t[TS_KEY_SIZE_BYTES] ts_doc_id;

// functions
void ts_doc_create(ts_env * env, ts_doc_id * id);
void ts_doc_del(ts_env * env, ts_doc_id * doc);
void ts_doc_tag(ts_env * env, ts_doc_id * doc, char * tag);
void ts_doc_untag(ts_env * env, ts_doc_id * doc, char * tag);

#endif

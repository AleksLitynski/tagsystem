#ifndef _TS_DOC_H_
#define _TS_DOC_H_

// includes
#include "tsenv.h"
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include <stdint.h>
#include "tsutil.h"

// macros

// types
typedef uint8_t ts_doc_id[TS_ID_BYTES];

// functions
void ts_doc_create(ts_env * env, ts_doc_id * id);
void ts_doc_del(ts_env * env, ts_doc_id * id);

void ts_doc_gen_id(ts_env * env, ts_doc_id * id);
char * ts_util_doc(ts_env * env, ts_doc_id * id);
char * ts_util_doc_dir(ts_env * env, ts_doc_id * id);

char * ts_util_str_id(ts_doc_id * id);
char * ts_util_str_id_bin(ts_doc_id * id);
char * ts_util_str_id_bin_split(ts_doc_id * id, char split, int sloc);

#endif
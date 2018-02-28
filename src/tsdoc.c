#include "tsdoc.h"
#include "../lib/lmdb/libraries/liblmdb/lmdb.h"
#include "../lib/sds/sds.h"
#include "tsid.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

// ts_doc_create(ts_doc * self, ts_db * db);
// ts_doc_delete(ts_doc * self);

// ts_doc_open(ts_doc * self, ts_db * db, ts_id id);
// ts_doc_close(ts_doc * self);

// ts_doc_tag(ts_doc * self, sds tag);
// ts_doc_untag(ts_doc * self, sds tag);
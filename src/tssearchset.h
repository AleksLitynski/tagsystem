#pragma once

// includes
#include "hash.h"
#include "tstags.h"
#include "tsdb.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tserror.h"
#include "tscli.h"

// macros

// types

// functions
ts_search * ts_searchset_create(ts_cli_ctx * ctx);
void ts_searchset_close(ts_search * search);
bool ts_searchset_has_one(ts_cli_ctx * ctx);
#ifndef _TS_H_
#define _TS_H_

#include "tsenv.h"
#include "tssearch.h"
#include "tsdoc.h"
#include "tstag.h"

// TSPATH=/default/db/location
// TSPWS=present+working+set

// iter(ts_ls, args(set), doc, { })
typedef struct {
    ts_env * env;
    ts_search * search;
    ts_doc_id * first;
    char * set;
    ts_tags * tags;
} ts_ls_ctx;  
typedef ts_doc_id * ts_ls_item;
void ts_ls_init(ts_ls_ctx * ctx, ts_ls_item * item);
void ts_ls_close(ts_ls_ctx * ctx, ts_ls_item * item);
int  ts_ls_next(ts_ls_ctx * ctx, ts_ls_item * item);



// make a file. Tag it with it's id (this way, you can always target a single doc)
void ts_mk0();
void ts_mk(ts_doc_id * id);  
void ts_rm();
// add a tag to each doc in the set
void ts_tag(char * tag); 
void ts_untag(char * tag);
// change the current set. +a+b+c to add a, b, c. a+b+c to set to a, b, c. -b+c to remove b, add c
void ts_cs(char * set);  

// Change tag system. Stored in $TTSPATH
void ts_cws(char * path); 
// get the present working set. Stored in $TSPWS
char * ts_pws();

// get the file path of a doc id.
// ALLOCATES MEMORY
char * ts_resolve(ts_doc_id * id);

#endif

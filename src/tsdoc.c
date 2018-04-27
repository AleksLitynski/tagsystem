#include "tsdoc.h"
#include "lmdb.h"
#include "sds.h"
#include "fs.h"
#include "tsid.h"
#include "tsdb.h"
#include "tserror.h"
#include "tstags.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

int ts_doc_create(ts_doc * self, ts_db * db) {

    // create a new document with a random id
    self->env = db;

    // creating a document means some operations will be
    // performed on the document, so start a transaction
    ts_db_begin_txn(self->env);

    // get an id
    ts_id_generate(&self->id, db);
    self->id_str = ts_id_string(&(self->id), sdsempty());

    // create file for the document
    self->dir = sdsdup(self->id_str);
    sdsrange(self->dir, 0, 1);

    self->dir = sdscatprintf(sdsempty(), "%s/%s", db->docs, self->dir);
    self->path = sdscatprintf(sdsempty(), "%s/%s", self->dir, self->id_str);

    fs_mkdir(self->dir, 0700);
    fs_open(self->path, "ab+");

    // add entry to 'index' table for the new doc
    MDB_val empty = {.mv_size = 0, .mv_data = ""};
    ts_db_put(db, "index", self->id_str, &empty);

    return TS_SUCCESS;
}

int ts_doc_delete(ts_doc * self) {

  // delete doc
  unlink(self->path);
  // delete folder if empty
  fs_rmdir(self->dir);

  // remove all tags
  ts_db_iter iter;
  ts_db_iter_open(&iter, self->env, "index", self->id_str);

  MDB_val next = { .mv_size = 0, .mv_data = 0};
  while(ts_db_iter_next(&iter, &next) != MDB_NOTFOUND) {
    ts_doc_untag(self, next.mv_data);
  }
  ts_db_iter_close(&iter);

  // remove list of tags
  ts_db_del(self->env, "index", self->id_str, NULL);

  return TS_SUCCESS;
}

int ts_doc_open(ts_doc * self, ts_db * db, ts_id id) {

  // get reference to existing document
  self->env = db;
  for(int i = 0; i < TS_ID_BYTES; i++) {
    self->id[i] = id[i];
  }

  // using a document almost always means db operations, so open a txn
  ts_db_begin_txn(self->env);

  self->id_str = ts_id_string(&(self->id), sdsempty());

  self->dir = sdsdup(self->id_str);
  sdsrange(self->dir, 0, 1);

  self->dir = sdscatprintf(sdsempty(), "%s/%s", db->docs, self->dir);
  self->path = sdscatprintf(sdsempty(), "%s/%s", self->dir, self->id_str);

  return TS_SUCCESS;
}

int ts_doc_close(ts_doc * self) {
  sdsfree(self->dir);
  sdsfree(self->path);
  sdsfree(self->id_str);

  // close doc's transaction
  ts_db_commit_txn(self->env);
}

int _ts_doc_has_tag(ts_doc * self, char * tag) {
  bool already_tagged = false;
  MDB_val next;
  ts_db_iter index_iter;

  // go through each tag on the document
  ts_db_iter_open(&index_iter, self->env, "index", self->id_str);
  while(ts_db_iter_next(&index_iter, &next) != MDB_NOTFOUND) {
    sds val = sdscatlen(sdsempty(), next.mv_data, next.mv_size);

    // if the tag already exists, set flag
    if(!strcmp(val, tag)) {
      already_tagged = true;
    }
    sdsfree(val);
  }
  ts_db_iter_close(&index_iter);
  
  return already_tagged;
}

int ts_doc_tag(ts_doc * self, char * tag) {

  // load the tag or create it if it doesn't exist
  ts_tags tags;
  int found_tag = ts_tags_open(&tags, self->env, tag);

  if(found_tag == TS_FAILURE) {
    ts_tags_empty(&tags);
  }

  // insert the new item
  ts_tags_insert(&tags, &(self->id));

  // save the tag
  ts_tags_write(&tags, self->env, tag);

  ts_tags_close(&tags);

  MDB_val new_tag = { .mv_size = strlen(tag), .mv_data = tag};
  
  // only add tag to document if it doesn't already have the tag
  if(!_ts_doc_has_tag(self, tag)) {
    ts_db_put(self->env, "index", self->id_str, &new_tag);
  }

  return TS_SUCCESS;
}

int ts_doc_untag(ts_doc * self, char * tag) {

  ts_tags tags;

  ts_db_del(self->env, "index", self->id_str, tag);

  // load the tag or exit it if it doesn't exist
  int found_tag = ts_tags_open(&tags, self->env, tag);
  if(found_tag == TS_FAILURE) {
    return TS_SUCCESS;
  }

  // remove the new item
  ts_tags_remove(&tags, &(self->id));

  // save the tag
  ts_tags_write(&tags, self->env, tag);

  ts_tags_close(&tags);
  return TS_SUCCESS;
}

#pragma once

// includes
#include "lmdb.h"
#include "sds.h"
#include "hash.h"

// macros

// types


// functions
int main(int argc, char * argv[]);

int list_cmd(int argc, char * argv[]);
int make_cmd(int argc, char * argv[]);
int remove_cmd(int argc, char * argv[]);
int tag_cmd(int argc, char * argv[]);
int changeset_cmd(int argc, char * argv[]);
int presentset_cmd(int argc, char * argv[]);
int help_cmd(int argc, char * argv[]);
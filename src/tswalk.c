#include "tswalk.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "tserror.h"


int ts_walk_create(ts_walk * self, ts_tags * tags) {
    self->source = tags;
    self->index = 0;
    self->history_depth = 0;
    self->history = calloc(sizeof(ts_walk_history_item), 1);
    self->history->current = tags->data;
    self->history->parent = 0;

    return TS_SUCCESS;
}

int ts_walk_close(ts_walk * self) {
    ts_walk_history_item * parent = 0;
    do {
        parent = self->history->parent;
        free(self->history); // free the parent list
        self->history = parent;

    } while(parent != 0);

    return TS_SUCCESS;
}

int ts_walk_push(ts_walk * self, int branch) {
    self->index++;

    // if this is a leaf, just increment the index and we're done
    if(self->history->current->type == TS_TAG_NODE_LEAF) return TS_SUCCESS;

    // if this is an inner node, push another node onto the history stack
    self->history_depth++;

    ts_walk_history_item * new_item = calloc(sizeof(ts_walk_history_item), 1);
    new_item->current = self->source->data + self->history->current->value.inner[branch];
    new_item->parent = self->history;
    self->history = new_item;

    return TS_SUCCESS;
}

int ts_walk_pop(ts_walk * self) {
    self->index--;

    // if we're still w/in a leaf node, just return
    if(self->history_depth <= self->index) {
        return TS_SUCCESS;
    }

    // pop the latest inner node from the history
    self->history_depth--;
    ts_walk_history_item * parent = self->history->parent;
    free(self->history);
    self->history = parent;

    return TS_SUCCESS;
}

bool ts_walk_test(ts_walk * self, int branch) {

    // if the tree is empty, we cannot continue
    if(self->source->data == 0) return false;

    // if leaf node and the current index isn't the same as the desired branch, return false
    if(self->history->current->type == TS_TAG_NODE_LEAF 
    && ts_id_get_bit(&(self->history->current->value.leaf), self->index) != branch) {
        return false;
    }

    // if inner node and the branch points to nothing, return false
    if(self->history->current->type == TS_TAG_NODE_INNER 
    && self->history->current->value.inner[branch] == 0) {
        return false;
    }

    // the leaf or inner node pointed to a valid next item
    return true;
}



/*
[ INFO     ] .
[ INFO     ] └── 1
[ INFO     ]     └── 0
[ INFO     ]         └── 0
[ INFO     ]             └── 0
[ INFO     ]                 └── 0
[ INFO     ]                     ├── 0
[ INFO     ]                     |   └── 10001011
[ INFO     ]                     └── 1
[ INFO     ]                         └── 10001110


insert appears to be broken. There's one too many inner nodes :(

*/
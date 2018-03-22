
#include "tssearch.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "tserror.h"


// functions
int ts_search_create(ts_search * self, ts_tags * tags, int tag_count) {
    self->index = 0;
    
    for(int i = 0; i < TS_ID_BITS; i++) {
        self->current_id[i] = 0;
    }
    
    self->walk_count = tag_count;
    self->walks = calloc(sizeof(ts_walk), tag_count);

    for(int i = 0; i < tag_count; i++) {
        ts_walk_create(self->walks + i, tags + i);
    }

    return TS_SUCCESS;
}

int ts_search_close(ts_search * self) {
    for(int i = 0; i < self->walk_count; i++) {
        ts_walk_close(self->walks +  i);
    }
    free(self->walks); // free the paths array
}

int _ts_search_pop(ts_search * self) {

    self->index--;

    // go back one and try the next branch
    self->current_id[self->index]++;

    for(int i = 0; i < self->walk_count; i++) {
        ts_walk_pop(self->walks + i);
    }

    return TS_SUCCESS;
}

int _ts_search_push(ts_search * self, int branch) {

    self->index++;
    // go forward one and reset the branch to 0
    self->current_id[self->index] = 0;
    for(int i = 0; i < self->walk_count; i++) {
        ts_walk_push(self->walks + i, branch);
    }

    return TS_SUCCESS;
}

bool _ts_search_test(ts_search * self, int branch) {

    for(int i = 0; i < self->walk_count; i++) {
        if(ts_walk_test(self->walks + i, branch) == false) return false;
    }
    return true;
}

int ts_search_step(ts_search * self, ts_id * id) {

    int branch = self->current_id[self->index];
    // LOG("%i.%i", self->index, branch);

    // If we tried both branches, pop up a layer
    // if we pop beyond layer 0, we're done
    while(branch == 2) {
        if(self->index == 0) {
            return TS_SEARCH_DONE;
        }
        _ts_search_pop(self);
        branch = self->current_id[self->index];
    }


    // check if the current item exists in all
    bool all_can_continue = _ts_search_test(self, branch);
    
    // if we reach the tip of a leaf, return the leaf and try the next branch
    if(all_can_continue && TS_ID_BITS - 1 == self->index) {
        // if the tags only diverge on the very last bit, we will be on an inner node,
        // not a leaf node. Jump to the leaf corosponding to the final branch before copying out the ID
        ts_tag_node * found_id = self->walks[0].history->current;
        if(found_id->type == TS_TAG_NODE_INNER) {
            found_id = self->walks[0].source->data + found_id->value.inner[branch];
        }
        
        ts_id_dup(&(found_id->value.leaf), id);
        self->current_id[self->index]++;
        return TS_SEARCH_FOUND;
    }

    // if we're not at the end and can go deeper, go deeper
    if(all_can_continue) {
        _ts_search_push(self, branch);
        return TS_SEARCH_NONE;
    }

    // if we can't go deeper, try the next branch
    if(!all_can_continue) {
        self->current_id[self->index]++;
        return TS_SEARCH_NONE;
    }

}

int ts_search_next(ts_search * self, ts_id * id) {
    while(true) {
        int next = ts_search_step(self, id);
        if(next == TS_SEARCH_NONE) continue;
        if(next == TS_SEARCH_FOUND) {
            return TS_SEARCH_FOUND;
        }
        if(next == TS_SEARCH_DONE) return TS_SEARCH_DONE;
    }
    return TS_SEARCH_DONE;
}

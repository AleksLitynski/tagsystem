
#include "tssearch.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "tserror.h"


// functions
int ts_search_create(ts_search * self, ts_db * db, ts_tags * tags, int tag_count) {
    self->index = 0;
    self-> branch = 0;
    
    self->tag_count = tag_count;
    self->tags = calloc(sizeof(ts_search_tag), tag_count);
    for(int i = 0; i < tag_count; i++) {
        self->tags[i].tags = tags + i;
        self->tags[i].current = tags->data;
        self->tags[i].parent = 0;
        self->tags[i].depth = 0;
    }

    return TS_SUCCESS;
}

int _ts_search_tag_close(ts_search_tag * tags) {
    if(tags->parent != 0) _ts_search_tag_close(tags->parent);
    free(tags);
    return TS_SUCCESS;
}

int ts_search_close(ts_search * self) {
    for(int i = 0; i < self->tag_count; i++) {
        _ts_search_tag_close(self->tags + i);
    }
    free(self->tags);
}

ts_search_tag * _ts_search_item_push(ts_search_tag * tags, int branch) {
    if(tags->current->type == TS_TAG_NODE_LEAF) return tags;

    ts_search_tag * new_tag = calloc(sizeof(ts_search_tag), 1);
    new_tag->tags = tags->tags;
    new_tag->current = tags->tags->data + tags->current->value.inner[branch];
    new_tag->parent = tags;
    new_tag->depth = tags->depth + 1;

    return new_tag;
}

ts_search_tag * _ts_search_item_pop(ts_search_tag * tags, int index) {
    
    if(tags->depth < index) return tags;

    ts_search_tag * parent = tags->parent;
    free(tags);
    return parent;
}

int ts_search_step(ts_search * self, ts_id * id) {


    // if we reach the tip of a leaf, return the leaf
    if(self->index >= 20) {
        id = &(self->tags[0].current->value.leaf);
        self->index--;
        self->branch = 1;
        for(int i = 0; i < self->tag_count; i++) {
            self->tags = _ts_search_item_pop(self->tags + 1, self->index);
        }
        return TS_SEARCH_FOUND;
    }

    // if go to index -1, return done
    if(self->index < 0) return TS_SEARCH_DONE;

    // check if the current item exists in all
    bool all_can_continue = true;
    for(int i = 0; i < self->tag_count; i++) {
        if(self->tags[i].current->type == TS_TAG_NODE_INNER && self->tags[i].current->value.inner[self->branch] != 0) {
            all_can_continue = false;
            break;
        }
        if(self->tags[i].current->type == TS_TAG_NODE_LEAF && ts_id_get_bit(&(self->tags[i].current->value.leaf), self->index) != self->branch) {
            all_can_continue = false;
            break;
        }
    }

    // if it does, go deeper
    if(all_can_continue) {
        self->index++;
        self->branch = 0;
        for(int i = 0; i < self->tag_count; i++) {
            self->tags = _ts_search_item_push(self->tags + 1, self->branch);
        }
        return TS_SEARCH_NONE;
    }

    // if it does not, go back a level
    if(!all_can_continue) {
        self->index--;
        self->branch = 1;
        for(int i = 0; i < self->tag_count; i++) {
            self->tags = _ts_search_item_pop(self->tags + 1, self->index);
        }
        return TS_SEARCH_NONE;
    }

}

int ts_search_next(ts_search * self, ts_id * id) {
    while(true) {
        int next = ts_search_step(self, id);
        if(next == TS_SEARCH_NONE) continue;
        if(next == TS_SEARCH_FOUND) TS_SEARCH_FOUND;
        if(next == TS_SEARCH_DONE) return TS_SEARCH_DONE;
    }
    return TS_SEARCH_DONE;
}

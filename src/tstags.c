
#include <stdlib.h>
#include <stddef.h>
#include "sds.h"
#include "tserror.h"
#include "tsid.h"
#include "tstags.h"

int ts_tags_empty(ts_tags ** self_addr) {
    _ts_tags_empty_sized(self_addr, 2);
    return TS_SUCCESS;
}

int _ts_tags_empty_sized(ts_tags ** self_addr, int size) {
    ts_tags * self = *self_addr;
    self = calloc(sizeof(ts_tags), 1);

    self->size = size;                          // start with one slot for the root and slot for an element
    self->occupied = 1;                         // always have a root element
    self->next = 1;                             // the next free slot is at 1
    self->data = calloc(size, sizeof(ts_tags)); // allocate the root and one free slot
    self->data->type = TS_TAG_NODE_INNER;       // not a leaf node
    return TS_SUCCESS;
}

size_t _ts_tags_insert_node(ts_tags ** self_addr, ts_tag_node * to_insert, size_t * node_addr) {
    ts_tags * self = * self_addr;

    // if there's no space, throw an error
    if(self->occupied >= self->size) return TS_TAGS_TREE_FULL;
    
    // remember where we're putting the new value
    *node_addr = self->next;

    // if the next slot is a hole in the data structure,
    // remember to fill whatever it points to
    if(self->data[self->next].type == TS_TAG_NODE_JUMP) {
        self->next = self->data[self->next].value.jump;
    } else {
        // if it wasn't a hole, we're at the last element.
        // just append another element to the end next insert.
        self->next = self->next + 1;
    }

    // insert the element
    self->data[*node_addr] = *to_insert;

    // increment occupied counter
    self->occupied++;

    // report we successfully inserted the node
    return TS_SUCCESS;
}

int _ts_tags_insert_no_resize(ts_tags ** self_addr, ts_id * id) {

    ts_tags * self = * self_addr;

    // parent of the root element is null
    ts_tag_node * parent = 0;
    int parent_branch = 0;

    // start traversing from root
    ts_tag_node * current = self->data;

    // create a node for this new element.
    // We may also need one or more inner nodes, 
    // but we know we need the leaf node

    // walk out the length of the tag
    for(int i = 0; i < TS_ID_BYTES; i++) {
        // does the tag branch left or right?
        int branch = ts_id_value(id, i);

        if(current->type == TS_TAG_NODE_LEAF) {
            // we've hit a leaf.
            // If we diverge from the leaf, add one inner node and attach
            // the new leaf and the existing leaf to each branch.
            // Otherwise, add a new inner node and repeat at the next index.
            // document ids should be unique, so eventually they will diverge.
            while(i < TS_ID_BYTES) {
                int current_value = ts_id_value(&current->value.leaf, i);
                int new_node_value = ts_id_value(id, i);
                if(current_value == new_node_value) {
                    // insert new parent
                    ts_tag_node common_parent;
                    common_parent.type = TS_TAG_NODE_INNER;
                    common_parent.value.inner[0] = 0;
                    common_parent.value.inner[1] = 0;
                    int res = _ts_tags_insert_node(self_addr, &common_parent, &parent->value.inner[parent_branch]);
                    if(res ==TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;

                    parent_branch = current_value;
                    // repeat
                } else {
                    ts_tag_node new_node;
                    new_node.type = TS_TAG_NODE_LEAF;
                    ts_id_dup(id, &new_node.value.leaf);
                    size_t new_node_addr;
                    int res = _ts_tags_insert_node(self_addr, &new_node, &new_node_addr);
                    if(res == TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;
                    
                    // the doc ids diverged. Insert one on the left and one on the right
                    parent->value.inner[current_value] = current - self->data;
                    parent->value.inner[new_node_value] = new_node_addr;
                    return TS_SUCCESS;
                }
                i++;
            }
        } else if(current->value.inner[branch] == 0) {
            // if we get an inner node and our path is unavailable,
            // create a leaf node and update inner node to point to leaf node
            ts_tag_node new_node;
            new_node.type = TS_TAG_NODE_LEAF;
            ts_id_dup(id, &new_node.value.leaf);
            size_t new_node_addr;
            int res = _ts_tags_insert_node(self_addr, &new_node, &new_node_addr);
            if(res == TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;

            current->value.inner[branch] = new_node_addr;            
            return TS_SUCCESS;
        } else {        
            // if we get an inner node and our path is available, follow the path
            parent = current;
            parent_branch = branch;
            current = self->data + current->value.inner[branch];
        }
    }

    return TS_SUCCESS;
}

int ts_tags_insert(ts_tags ** self_addr, ts_id * id) {

    // assume there was no space and we may need to grow the tree
    int had_space = false;
    while(!had_space) {
        // attempt to grow the tree
        _ts_tags_resize(self_addr, 1);
        // attempt to insert the element
        int res = _ts_tags_insert_no_resize(self_addr, id);
        // we may need to insert inner nodes and the tree may need
        // to grow further. If there wasn't enough space, we re-try with a bigger tree
        had_space = res != TS_TAGS_TREE_FULL;
    }

    return TS_SUCCESS;
}

int _ts_tags_remove_node(ts_tags ** self_addr, size_t node_addr) {
    ts_tags * self = *self_addr;
    
    // Empty nodes are a linked list of unoccupied nodes.
    // The last node in the chain will always be the last occupied node
    // in self->data.

    // create a new empty node to replace the deleted node
    ts_tag_node empty_node;
    empty_node.type = TS_TAG_NODE_JUMP;
    // empty node points to the current 'next' node
    empty_node.value.jump = self->next;

    // overwrite the deleted node
    *(self->data + node_addr) = empty_node;
    // next now points to the deleted node
    self->next = node_addr;

    // one less node is occupied
    self->occupied--;

    return TS_SUCCESS;
}

int ts_tags_remove(ts_tags ** self_addr, ts_id * id) {
    ts_tags * self = *self_addr;

    _ts_tags_remove_recursive(self_addr, id, 0, 0);

    // if we're more than half unoccupied, the tree will be 
    // repacked into a smaller space
    _ts_tags_resize(&self, 0);
    return TS_SUCCESS;
}

int _ts_tags_remove_recursive(ts_tags ** self_addr, ts_id * id, size_t node_addr, int idx) {
    ts_tags * self = *self_addr;

    ts_tag_node * current = self->data + node_addr;

    // if the node is a leaf, delete it and we're done
    if(current->type == TS_TAG_NODE_LEAF) {
        _ts_tags_remove_node(self_addr, node_addr);
    }
    // if the node is an inner, recurse left or right, then if both childen are 0, delete it
    else if(current->type == TS_TAG_NODE_INNER) {
        int branch = ts_id_value(id, idx);

        _ts_tags_remove_recursive(self_addr, id, current->value.inner[branch], idx + 1);
        current->value.inner[branch] = 0;
        if(current->value.inner[0] == 0 && current->value.inner[1] == 0) {
            _ts_tags_remove_node(self_addr, node_addr);
        }
    }

    return TS_SUCCESS;

}

int _ts_tags_copy(ts_tags ** self_addr, ts_tags * source, size_t source_idx) {
    ts_tag_node * current = source->data + source_idx;
    // recursive function to insert all items in source into self
    if(current->type == TS_TAG_NODE_LEAF) {
        // if this is a leaf, insert the value
        _ts_tags_insert_no_resize(self_addr, &(current->value.leaf));
    }
    else if(current->type == TS_TAG_NODE_INNER) {
        // if this is an inner node, recurse left/right if left/right exist
        if(current->value.inner[0] != 0) {
            _ts_tags_copy(self_addr, source, current->value.inner[0]);
        }
        if(current->value.inner[1] != 0) {
            _ts_tags_copy(self_addr, source, current->value.inner[1]);
        }
    }

}

int _ts_tags_resize(ts_tags ** self_addr, int delta) {
    ts_tags * self = *self_addr;

    int new_size = 0;
    if(self->occupied + delta > self->size) {
        new_size = self->size * 2;
    }

    if(self->occupied + delta < self->size / 2) {
        new_size = self->size / 2;
    }

    if(new_size != 0) {
        ts_tags * new_tags = malloc(sizeof(ts_tags));
        _ts_tags_empty_sized(&new_tags, new_size);

        // copy over all items
        _ts_tags_copy(&new_tags, self, 0);

        // free the old one, overwrite with the new one
        ts_tags_close(self_addr);
        self = new_tags;
    }

    return TS_SUCCESS;
}

int ts_tags_close(ts_tags ** self_addr) {
    ts_tags * self = *self_addr;
    free(self->data);
    free(self);
}

sds _ts_tags_print_node(ts_tags ** self_addr, size_t node_addr, sds padding, sds printed) {
    ts_tags * self = *self_addr;
    /*
            . 
            ├── 0
            │   ├── abc123
            |   └── abc124
            └── 1
    */

    ts_tag_node * current = self->data + node_addr;

    if(current->type == TS_TAG_NODE_LEAF) {
        sds id_str;
        ts_id_string(&(current->value.leaf), id_str);
        printed = sdscatprintf(printed, "%s└── %s\n", padding, id_str);
        sdsfree(id_str);
    }

    if(current->type == TS_TAG_NODE_INNER) {

        sds extendedpadding = sdscatprintf(sdsempty(), "%s|   ", padding);
        if(current->value.inner[0] != 0) {
            // print first when second exists
            char * tee = "├";
            // print when second doesn't exist
            if(current->value.inner[1] == 0) tee = "└";
            
            printed = sdscatprintf(printed, "%s%s── 0\n", padding, tee);
            printed = _ts_tags_print_node(self_addr, current->value.inner[0], extendedpadding, printed);
            
        }

        // print second
        if(current->value.inner[1] != 0) {
            printed = sdscatprintf(printed, "%s└── 1\n", padding);
            printed = _ts_tags_print_node(self_addr, current->value.inner[1], extendedpadding, printed);
        }

        sdsfree(extendedpadding);
    }

    return printed;
}

sds ts_tags_print(ts_tags ** self_addr, sds printed) {
    ts_tags * self = *self_addr;

    // print size, occupied, and all 'next' values
    printed = sdscatprintf(printed, "\nsize: %i\n", self->size);
    printed = sdscatprintf(printed, "occupied: %i\n", self->occupied);

    // print the chain of empty cells
    size_t next = self->next;
    printed = sdscatprintf(printed, "nexts: ");
    do {
        printed = sdscatprintf(printed, "%zu, ", next);
        next = (self->data + next)->value.jump;
    } while(next != 0);
    printed = sdscatprintf(printed, "\n");

    // print the actual tree structure
 
    printed = sdscatprintf(printed, ".\n");
    sds padding = sdsempty();
    _ts_tags_print_node(self_addr, 0, padding, printed);
    sdsfree(padding);

    return printed;
}
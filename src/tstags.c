
#include <stdlib.h>
#include <stddef.h>
#include "sds.h"
#include "tserror.h"
#include "tsid.h"
#include "tstags.h"

int ts_tags_empty(ts_tags * self) {
    _ts_tags_empty_sized(self, 2);
    return TS_SUCCESS;
}

int _ts_tags_empty_sized(ts_tags * self, int size) {
    self->size = size;                          // start with one slot for the root and slot for an element
    self->occupied = 1;                         // always have a root element
    self->next = 1;                             // the next free slot is at 1
    self->data = calloc(size, sizeof(ts_tags)); // allocate the root and one free slot
    self->data->type = TS_TAG_NODE_INNER;       // not a leaf node
    return TS_SUCCESS;
}

size_t ts_tags_insert_node(ts_tags * self, ts_tag_node * to_insert) {

    // make sure there's space before we insert the new node
    _ts_tags_resize(&self, 1);

    // remember where we're putting the new value
    size_t next = self->next;

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
    self->data[next] = *to_insert;

    // increment occupied counter
    self->occupied++;

    // return the location of the new element
    return next;
}

int ts_tags_insert(ts_tags * self, ts_id * id) {

    // parent of the root element is null
    ts_tag_node * parent = 0;
    int parent_branch = 0;

    // start traversing from root
    ts_tag_node * current = self->data;

    // create a node for this new element.
    // We may also need one or more inner nodes, 
    // but we know we need the leaf node
    ts_tag_node new_node;
    new_node.type = TS_TAG_NODE_LEAF;
    ts_id_dup(id, &new_node.value.leaf);
    size_t new_node_addr = ts_tags_insert_node(self, &new_node);

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
            for( ; i < TS_ID_BYTES; i++) {
                int current_value = ts_id_value(&current->value.leaf, i);
                int new_node_value = ts_id_value(&new_node.value.leaf, i);
                if(current_value == new_node_value) {
                    // insert new parent
                    ts_tag_node common_parent;
                    common_parent.type = TS_TAG_NODE_INNER;
                    parent->value.inner[parent_branch] = ts_tags_insert_node(self, &common_parent);
                    parent_branch = current_value;
                    // repeat
                } else {
                    // the doc ids diverged. Insert one on the left and one on the right
                    parent->value.inner[current_value] = self->data - current;
                    parent->value.inner[new_node_value] = new_node_addr;
                    return TS_SUCCESS;
                }
            }
        } else if(current->value.inner[branch] == 0) {
            // if we get an inner node and our path is unavailable,
            // create a leaf node and update inner node to point to leaf node
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

int _ts_tags_remove_node(ts_tags * self, size_t node_addr) {
    
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
    // if we're more than half unoccupied, the tree will be 
    // repacked into a smaller space
    _ts_tags_resize(&self, 0);
    return TS_SUCCESS;
}

int ts_tags_remove(ts_tags * self, ts_id * id) {

    _ts_tags_remove_recursive(self, id, 0, 0);
    return TS_SUCCESS;
}

int _ts_tags_remove_recursive(ts_tags * self, ts_id * id, size_t node_addr, int idx) {

    ts_tag_node * current = self->data + node_addr;
    int branch = ts_id_value(id, idx);

    // if the node is a leaf, delete it and we're done
    if(current->type == TS_TAG_NODE_LEAF) {
        _ts_tags_remove_node(self, node_addr);
    }
    // if the node is an inner, recurse left or right, then if both childen are 0, delete it
    if(current->type == TS_TAG_NODE_INNER) {
        _ts_tags_remove_recursive(self, id, current->value.inner[branch], idx + 1);
        current->value.inner[branch] = 0;
        if(current->value.inner[0] == 0 && current->value.inner[1] == 0) {
            _ts_tags_remove_node(self, node_addr);
        }
    }

    return TS_SUCCESS;

}

int _ts_tags_copy(ts_tags * self, ts_tag_node * source) {
    // recursive function to insert all items in source into self
    if(source->type == TS_TAG_NODE_LEAF) {
        // if this is a leaf, insert the value
        ts_tags_insert(self, &(source->value.leaf));
    }
    if(source->type == TS_TAG_NODE_INNER) {
        // if this is an inner node, recurse left/right if left/right exist
        if(source->value.inner[0] != 0) {
            _ts_tags_copy(self, self->data + source->value.inner[0]);
        }
        if(source->value.inner[1] != 0) {
            _ts_tags_copy(self, self->data + source->value.inner[1]);
        }
    }
}

int _ts_tags_resize(ts_tags ** self, int delta) {
    ts_tags * current_self = *self;

    int new_size = 0;
    if(current_self->occupied + delta > current_self->size) {
        new_size = current_self->size * 2;
    }

    if(current_self->occupied + delta < current_self->size / 2) {
        new_size = current_self->size / 2;
    }

    if(new_size != 0) {
        ts_tags * new_tags = malloc(sizeof(ts_tags));
        _ts_tags_empty_sized(new_tags, new_size);

        // copy over all items
        _ts_tags_copy(new_tags, current_self->data);

        // free the old one, overwrite with the new one
        ts_tags_close(*self);
        *self = new_tags;
    }

    return TS_SUCCESS;
}

int ts_tags_close(ts_tags * self) {
    free(self->data);
}

sds _ts_tags_print_node(ts_tags * self, size_t node_addr, sds padding, sds printed) {
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
            printed = _ts_tags_print_node(self, current->value.inner[0], extendedpadding, printed);
            
        }

        // print second
        if(current->value.inner[1] != 0) {
            printed = sdscatprintf(printed, "%s└── 1\n", padding);
            printed = _ts_tags_print_node(self, current->value.inner[1], extendedpadding, printed);
        }

        sdsfree(extendedpadding);
    }

    return printed;
}

sds ts_tags_print(ts_tags * self, sds printed) {
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
    _ts_tags_print_node(self, 0, padding, printed);
    sdsfree(padding);

    return printed;
}
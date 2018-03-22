
#include <stdlib.h>
#include <stdio.h>
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

size_t _ts_tags_insert_node(ts_tags * self, ts_tag_node * to_insert, /* out value */ size_t * node_addr) {

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

int _ts_tags_insert_no_resize(ts_tags * self, ts_id * id) {

    // parent of the root element is null
    ts_tag_node * parent = 0;
    int parent_branch = 0;

    // start traversing from root
    ts_tag_node * current = self->data;

    // create a node for this new element.
    // We may also need one or more inner nodes, 
    // but we know we need the leaf node

    // walk out the length of the tag
    for(int i = 0; i < TS_ID_BITS; i++) {
        // does the tag branch left or right?
        int branch = ts_id_get_bit(id, i);

        // TODO: randomly (~1/10 times) current will be pointing to the wrong data even though self->data is defined 
        if(current->type == TS_TAG_NODE_LEAF) {

            // the item already exists in the tree, no need to add it again
            if(ts_id_eq(&current->value.leaf, id)) return TS_SUCCESS;
            
            
            // we've hit a leaf.
            // If we diverge from the leaf, add one inner node and attach
            // the new leaf and the existing leaf to each branch.
            // Otherwise, add a new inner node and repeat at the next index.
            // document ids should be unique, so eventually they will diverge.
            while(i < TS_ID_BITS) {
                int current_value = ts_id_get_bit(&current->value.leaf, i);
                int new_node_value = ts_id_get_bit(id, i);
                if(current_value == new_node_value) {
                    // insert new parent. New Parent has a branch pointing towards existing child
                    ts_tag_node common_parent;
                    common_parent.type = TS_TAG_NODE_INNER;
                    common_parent.value.inner[0] = 0;
                    common_parent.value.inner[1] = 0;
                    common_parent.value.inner[current_value] = current - self->data;

                    // insert new parent. Set correct branch of current parent to new parent
                    int res = _ts_tags_insert_node(self, &common_parent, &parent->value.inner[parent_branch]);
                    // if we were unable to allocate new parent, throw that we need more space
                    if(res == TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;
                    
                    // set current parent to new parent and set branch to new branch
                    parent = self->data + parent->value.inner[parent_branch];
                    parent_branch = current_value;
                    // repeat
                } else {

                    ts_tag_node common_parent;
                    common_parent.type = TS_TAG_NODE_INNER;
                    common_parent.value.inner[0] = 0;
                    common_parent.value.inner[1] = 0;
                    common_parent.value.inner[current_value] = current - self->data;
                    int res2 = _ts_tags_insert_node(self, &common_parent, &parent->value.inner[parent_branch]);
                    if(res2 == TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;

                    ts_tag_node new_node;
                    new_node.type = TS_TAG_NODE_LEAF;
                    ts_id_dup(id, &new_node.value.leaf);
                    size_t new_node_addr;
                    int res = _ts_tags_insert_node(self, &new_node, &new_node_addr);
                    if(res == TS_TAGS_TREE_FULL) return TS_TAGS_TREE_FULL;
                    
                    // the doc ids diverged. Insert one on the left and one on the right
                    // common_parent.value.inner[current_value] = current - self->data;
                    ts_tag_node * inserted_common_parent = self->data + parent->value.inner[parent_branch];
                    inserted_common_parent->value.inner[new_node_value] = new_node_addr;
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
            int res = _ts_tags_insert_node(self, &new_node, &new_node_addr);
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

int ts_tags_insert(ts_tags * self, ts_id * id) {

    // assume there was no space and we may need to grow the tree
    int had_space = false;
    while(!had_space) {
        // attempt to grow the tree
        _ts_tags_resize(self, 1);
        // attempt to insert the element
        int res = _ts_tags_insert_no_resize(self, id);
        // we may need to insert inner nodes and the tree may need
        // to grow further. If there wasn't enough space, we re-try with a bigger tree
        had_space = res != TS_TAGS_TREE_FULL;
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

    return TS_SUCCESS;
}

int ts_tags_remove(ts_tags * self, ts_id * id) {

    _ts_tags_remove_recursive(self, id, 0, 0);

    // if we're more than half unoccupied, the tree will be 
    // repacked into a smaller space
    _ts_tags_resize(self, 0);
    return TS_SUCCESS;
}

size_t _ts_tags_remove_recursive(ts_tags * self, ts_id * id, size_t node_addr, int idx) {

    // it's possible we're removing an item that's not in the tree. If so, just return
    if(idx >= TS_ID_BITS) return node_addr;

    ts_tag_node * current = self->data + node_addr;

    // if the node is a leaf and it's actually the leaf we're looking for, delete it and we're done
    if(current->type == TS_TAG_NODE_LEAF && ts_id_eq(&current->value.leaf, id)) {
        _ts_tags_remove_node(self, node_addr);
        return 0;
    }

    // if we hit the end and we're still on an inner node, the inner node will point to the leaf to remove
    if(current->type == TS_TAG_NODE_INNER && idx == TS_ID_BITS - 1) {
        int branch = ts_id_get_bit(id, TS_ID_BITS - 1);
        size_t leaf_addr = current->value.inner[branch];
        if(leaf_addr != 0) {
            _ts_tags_remove_node(self, leaf_addr);
            current->value.inner[branch] = 0;
        }

        if(current->value.inner[0] == 0 && current->value.inner[1] == 0) {
            _ts_tags_remove_node(self, node_addr);
            return 0;
        }

        return node_addr;
    }

    // if the node is an inner, recurse left or right, then if both childen are 0, delete it
    if(current->type == TS_TAG_NODE_INNER) {
        int branch = ts_id_get_bit(id, idx);
        int off_branch = branch == 1 ? 0 : 1;

        size_t branch_addr = _ts_tags_remove_recursive(self, id, current->value.inner[branch], idx + 1);
        current->value.inner[branch] = branch_addr;
        size_t off_branch_addr = current->value.inner[off_branch];

        if(node_addr == 0) return node_addr;

        if(branch_addr == 0) {
            _ts_tags_remove_node(self, node_addr);
            return off_branch_addr;
        }

        if(off_branch_addr == 0) {
            _ts_tags_remove_node(self, node_addr);
            return branch_addr;

        }
    }

    return node_addr;

}

int _ts_tags_copy(ts_tags * self, ts_tag_node * source, size_t current_addr) {
    ts_tag_node * current = source + current_addr;
    // recursive function to insert all items in source into self
    if(current->type == TS_TAG_NODE_LEAF) {
        // if this is a leaf, insert the value
        _ts_tags_insert_no_resize(self, &(current->value.leaf));
    }
    else if(current->type == TS_TAG_NODE_INNER) {
        // if this is an inner node, recurse left/right if left/right exist
        if(current->value.inner[0] != 0) {
            _ts_tags_copy(self, source, current->value.inner[0]);
        }
        if(current->value.inner[1] != 0) {
            _ts_tags_copy(self, source, current->value.inner[1]);
        }
    }

}

int _ts_tags_resize(ts_tags * self, int delta) {

    int new_size = 0;
    if(self->occupied + delta > self->size) {
        new_size = self->size * 2;
    }

    if(self->occupied + delta < self->size / 2) {
        new_size = self->size / 2;
    }

    if(new_size != 0) {
        // store a reference to existing tree and zero out self
        ts_tag_node * old_data = self->data;
        _ts_tags_empty_sized(self, new_size);

        // copy over all items
        _ts_tags_copy(self, old_data, 0);

        // free the old data
        free(old_data);
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
        sds id_str = ts_id_nbit_string(&(current->value.leaf), sdsempty(), 8);
        printed = sdscatprintf(printed, "%s+-- %s\n", padding, id_str); // sdscatprintf(printed, "%s└── %s\n", padding, id_str);
        sdsfree(id_str);
    }

    if(current->type == TS_TAG_NODE_INNER) {

        sds extendedpadding = sdscatprintf(sdsempty(), "%s|   ", padding);
        sds emptypadding = sdscatprintf(sdsempty(), "%s    ", padding);
        if(current->value.inner[0] != 0) {
            // print first when second exists
            char * tee = "|"; // "├";
            sds currentpadding = extendedpadding;
            // print when second doesn't exist
            if(current->value.inner[1] == 0) {
                tee = "+"; // "└";
                currentpadding = emptypadding;
            }
            
            printed = sdscatprintf(printed, "%s%s-- 0\n", padding, tee); // sdscatprintf(printed, "%s%s── 0\n", padding, tee);
            printed = _ts_tags_print_node(self, current->value.inner[0], currentpadding, printed);
            
        }

        // print second
        if(current->value.inner[1] != 0) {
            printed = sdscatprintf(printed, "%s+-- 1\n", padding); // sdscatprintf(printed, "%s└── 1\n", padding);
            printed = _ts_tags_print_node(self, current->value.inner[1], emptypadding, printed);
        }

        sdsfree(extendedpadding);
        sdsfree(emptypadding);
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
    } while((self->data + next)->type == TS_TAG_NODE_JUMP);
    printed = sdscatprintf(printed, "\n");

    // print the actual tree structure
 
    printed = sdscatprintf(printed, ".\n");
    sds padding = sdsempty();
    printed = _ts_tags_print_node(self, 0, padding, printed);
    sdsfree(padding);

    return printed;
}

void ts_tags_log(ts_tags * tags) {
    sds str = ts_tags_print(tags, sdsempty());                      
    int count;                                                      
    sds * lines = sdssplitlen(str, sdslen(str), "\n", 1, &count);   
    for(int i = 0; i < count; i++) {                                
        LOG("%s", lines[i]);                                        
    }                                                               
    sdsfreesplitres(lines, count);
}
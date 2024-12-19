#include <stdlib.h>
#include <string.h>

#include "structures.h"

L2 create_L2_empty(){
    L2 node;
    node.ind   = -1;
    node.by    = -1;
    node.count = 0;
    node.head  = NULL;
    node.tail  = NULL;
    return node;
}

L2 create_L2(int ind_in, int count_in, int by_in, L3* head_in, L3* tail_in){
    L2 node;
    node.ind   = ind_in;
    node.count = count_in;
    node.by    = by_in;
    node.head  = head_in;
    node.tail  = tail_in;
    return node;
}

L3* create_L3_empty(){
    L3* node   = (L3*) malloc(sizeof(L3));
    node->ind  = -1;
    node->next = NULL;
    return node;
}

L3* create_L3(int ind_in, L3* next_in){
    L3* node   = (L3*) malloc(sizeof(L3));
    node->ind  = ind_in;
    node->next = next_in;
    return node;
}

void free_L2(L2 node){
    if (node.head) free_L3(node.head);
}

void free_L3(L3* node){
    if (!node)      return;
    if (node->next) free_L3(node->next);
    free(node);
}

L2 copy_L2(L2 node) {
    L2 new_node = create_L2(node.ind, node.count, node.by, NULL, NULL);
    if (node.head) 
    {
        new_node.head = copy_L3(node.head);
        L3 *current = new_node.head;
        while (current->next != NULL) {current = current->next;}
        new_node.tail = current;
    } 
    else {new_node.head = new_node.tail = NULL;}
    return new_node;
}

L3* copy_L3(L3* node) {
    if (!node) return NULL;
    L3* new_node = create_L3(node->ind, NULL);
    if (node->next) new_node->next = copy_L3(node->next);
    return new_node;
}

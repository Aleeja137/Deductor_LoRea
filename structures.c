#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"

typedef struct {
    unsigned m;
    unsigned *columns;
    unsigned *mapping;
} matrix_schema;



struct mat_schema

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


int timespec_subtract (struct timespec *result, struct timespec *x, struct timespec *y)
{
    if (x->tv_nsec < y->tv_nsec)
    {
        int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
        y->tv_nsec -= 1000000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_nsec - y->tv_nsec > 1000000000)
    {
        int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
        y->tv_nsec += 1000000000 * nsec;
        y->tv_sec -= nsec;
    }

    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_nsec = x->tv_nsec - y->tv_nsec;

    return x->tv_sec < y->tv_sec;
}

// void print_elapsed (struct timespec elapsed)
// {
//     if (elapsed.tv_sec <= 0)
//     printf("%ld ns\n",elapsed.tv_nsec);
//     else
// }
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"


mgu_schema* create_empty_mgu_schema(const unsigned m) {
    mgu_schema *ms = malloc(sizeof(mgu_schema));
    if (!ms) {fprintf(stderr, "Unable to allocate memory for mgu_schema\n"); exit(EXIT_FAILURE); }
    ms->m = m;
    ms->columns = (unsigned*)malloc(m * sizeof(unsigned));
    ms->mapping_L = (unsigned*)malloc(m * sizeof(unsigned));
    ms->mapping_R = (unsigned*)malloc(m * sizeof(unsigned));

    if (!ms->columns || !ms->mapping_L || !ms->mapping_R) {
        fprintf(stderr, "Unable to allocate memory inside mgu_schema\n"); 
        free(ms->columns); free(ms->mapping_L); free(ms->mapping_R); free(ms);
        exit(EXIT_FAILURE);
    }

    return ms;
}

// Assumes input arrays are not null and have appropriate size
mgu_schema* create_mgu_schema(const unsigned m, unsigned* columns, unsigned* mappings_L, unsigned* mappings_R) {
    mgu_schema *ms = malloc(sizeof(mgu_schema));
    if (!ms) {fprintf(stderr, "Unable to allocate memory for mgu_schema\n"); exit(EXIT_FAILURE); }
    ms->m = m;
    ms->columns = (unsigned*)malloc(m * sizeof(unsigned));
    ms->mapping_L = (unsigned*)malloc(m * sizeof(unsigned));
    ms->mapping_R = (unsigned*)malloc(m * sizeof(unsigned));

    if (!ms->columns || !ms->mapping_L || !ms->mapping_R) {
        fprintf(stderr, "Unable to allocate memory inside mgu_schema\n"); 
        free(ms->columns); free(ms->mapping_L); free(ms->mapping_R); free(ms);
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < m; i++) {
        ms->columns[i] = columns[i]; 
        ms->mapping_L[i] = mappings_L[i];
        ms->mapping_R[i] = mappings_R[i];
    }

    return ms;
}

void free_mgu_schema(mgu_schema* ms) {
    if (ms) {
        free(ms->mapping_L);
        free(ms->mapping_R);
        free(ms->columns);
        free(ms);
    }
}

void print_mgu_schema(mgu_schema* ms) {
    printf("columns: [");
    for (unsigned i = 0; i < ms->m - 1; i++) {
        printf("%d, ", ms->columns[i]);
    }    
    printf("%d]\n", ms->columns[ms->m - 1]);

    printf("mapping_L: [");
    for (unsigned i = 0; i < ms->m - 1; i++) {
        printf("%d, ", ms->mapping_L[i]);
    }    
    printf("%d]\n", ms->mapping_L[ms->m - 1]);

    printf("mapping_R: [");
    for (unsigned i = 0; i < ms->m - 1; i++) {
        printf("%d, ", ms->mapping_R[i]);
    }    
    printf("%d]\n", ms->mapping_R[ms->m - 1]);
}


matrix_schema* create_empty_matrix_schema(const unsigned m) {
    matrix_schema *ms = malloc(sizeof(matrix_schema));
    if (!ms) {fprintf(stderr, "Unable to allocate memory for matrix_schema\n"); exit(EXIT_FAILURE); }
    ms->m = m;
    ms->columns = (unsigned*)malloc(m*sizeof(unsigned));
    ms->mapping = (unsigned*)malloc(m*sizeof(unsigned));

    if (!ms->columns || !ms->mapping) {
        fprintf(stderr, "Unable to allocate memory inside matrix_schema\n"); 
        free(ms->columns); free(ms->mapping); free(ms);
        exit(EXIT_FAILURE);
    }

    return ms;
}

// Assumes input arrays are not null and have appropiate size
matrix_schema* create_matrix_schema(const unsigned m, unsigned* columns, unsigned* mappings) {
    matrix_schema *ms = malloc(sizeof(matrix_schema));
    if (!ms) {fprintf(stderr, "Unable to allocate memory for matrix_schema\n"); exit(EXIT_FAILURE); }
    ms->m = m;
    ms->columns = (unsigned*)malloc(m*sizeof(unsigned));
    ms->mapping = (unsigned*)malloc(m*sizeof(unsigned));

    if (!ms->columns || !ms->mapping) {
        fprintf(stderr, "Unable to allocate memory inside matrix_schema\n"); 
        free(ms->columns); free(ms->mapping); free(ms);
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i<m; i++){
        ms->columns[i]=columns[i]; 
        ms->mapping[i]=mappings[i];
    }

    return ms;
}

void free_matrix_schema(matrix_schema* ms) {
    if (ms) {
        free(ms->mapping);
        free(ms->columns);
        free(ms);
    }
}

void print_matrix_schema(matrix_schema* ms){
    printf("columns: [");
    for (unsigned i = 0; i<ms->m-1; i++){
        printf("%d, ",ms->columns[i]);
    }    
    printf("%d]\n",ms->columns[ms->m-1]);

    printf("mapping: [");
    for (unsigned i = 0; i<ms->m-1; i++){
        printf("%d, ",ms->mapping[i]);
    }    
    printf("%d]\n",ms->mapping[ms->m-1]);
}

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
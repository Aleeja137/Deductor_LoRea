#include <stdlib.h>

#include "structures.h"

L1* create_L1_empty(){
    L1* node   = (L1*) malloc(sizeof(L1));
    node->val  = 0;
    node->info = NULL;
    return node; 
}

L1* create_L1(int val_in, L2 *info_in){
    L1* node   = (L1*) malloc(sizeof(L1));
    node->val  = val_in;
    node->info = info_in;
    return node;
}

L2* create_L2_empty(){
    L2* node   = (L2*) malloc(sizeof(L2));
    node->val  = 0;
    node->head = NULL;
    node->tail = NULL;
    node->me   = NULL;
    return node;
}

L2* create_L2(int val_in, L3* head_in, L3* tail_in, L1* me_in){
    L2* node   = (L2*) malloc(sizeof(L2));
    node->val  = val_in;
    node->head = head_in;
    node->tail = tail_in;
    node->me   = me_in;
    return node;
}

L3* create_L3_empty(){
    L3* node   = (L3*) malloc(sizeof(L3));
    node->var  = NULL;
    node->next = NULL;
    return node;
}

L3* create_L3(L1* var_in, L3* next_in){
    L3* node   = (L3*) malloc(sizeof(L3));
    node->var  = var_in;
    node->next = next_in;
    return node;
}

// Crear matriz NxM vacía
L1** constr_mat_vacia(size_t n, size_t m){
    L1** mat = (L1**) malloc(n*m*sizeof(L1*));
    size_t i;
    for (i=0; i<n*m; i++) mat[i] = create_L1_empty();
    return mat;
}


void free_L1(L1* node){
    if (!node)      return;
    if (node->info) free_L2(node->info);
    free(node);
}

void free_L2(L2* node){
    if (!node)      return;
    if (node->head) free_L3(node->head);
    if (node->tail) free_L3(node->tail);
    // if (node->me)   free_L1(node->me);
    free(node);
}

void free_L3(L3* node){
    if (!node)      return;
    if (node->var)  free_L1(node->var);
    if (node->next) free_L3(node->next);
    free(node);
}


void free_L1_mat(int n, int m, L1** mat){
    int i;
    for (i=0; i<n*m; i++) free_L1(mat[i]);
    free(mat);
}


#include <stdlib.h>
#include <string.h>

#include "structures.h"

L1* create_L1_empty(){
    L1* node   = (L1*) malloc(sizeof(L1));
    node->val  = 0;
    node->info = NULL;
    node->name = "empty";
    return node; 
}

L1* create_L1(int val_in, L2 *info_in, char *name_in){
    L1* node   = (L1*) malloc(sizeof(L1));
    node->val  = val_in;
    node->info = info_in;
    node->name = strdup(name_in);
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
    if (node->name) free(node->name);
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


L1* copy_L1(L1* node) {
    // printf("Copying L1 %s\n",node->name); // Check
    if (!node) return NULL;
    // printf("Check 1\n"); // Check
    char * tmp_name = strdup(node->name);
    // printf("Check 2\n"); // Check
    L1* new_node = create_L1(node->val, NULL, tmp_name);
    // printf("Check 3\n"); // Check
    free(tmp_name);
    // printf("Check 4\n"); // Check
    if (node->info) new_node->info = copy_L2(node->info);
    // printf("Check 5\n"); // Check
    if (new_node->info != NULL) new_node->info->me = new_node;
    // printf("COPIED L1  %s\n",node->name); // Check
    return new_node;
}

L2* copy_L2(L2* node) {
    // printf("Copying L2 %s\n",node->me->name); // Check
    if (!node) return NULL;
    // printf("Check 1\n"); // Check
    L2* new_node = create_L2(node->val, NULL, NULL, NULL);
    // printf("Check 2\n"); // Check
    if (node->head) new_node->head = copy_L3(node->head);
    // printf("Check 3\n"); // Check
    if (new_node->head != NULL) new_node->head->var=node->head->var;
    // printf("Check 4\n"); // Check
    if (node->tail) new_node->tail = copy_L3(node->tail);
    // if (node->me) new_node->me = copy_L1(node->me); // Revise: Sin copia circular
    // printf("COPIED L2  %s\n",node->me->name); // Check
    return new_node;
}

L3* copy_L3(L3* node) {
    // printf("Copying L3 %s\n",node->var->name); // Check
    if (!node) return NULL;
    L3* new_node = create_L3(NULL, NULL);
    // if (node->var) new_node->var = copy_L1(node->var); // Revise: Sin copia circular
    if (node->next) new_node->next = copy_L3(node->next);
    // printf("COPIED L3  %s\n",node->var->name); // Check
    return new_node;
}

L1** copy_mat(size_t n, size_t m, L1** mat) {
    // printf("n = %zu, m = %zu\n", n, m); // Check
    // printf("Check interno 1\n"); // Check
    L1** mat2 = (L1**) malloc(n * m * sizeof(L1*));
    // printf("Check interno 2\n"); // Check
    if (!mat2) return NULL; // Allocation failure
    // printf("Check interno 3\n"); // Check
    
    for (size_t i = 0; i < n * m; i++) {
        mat2[i] = copy_L1(mat[i]);
    }
    return mat2;
}

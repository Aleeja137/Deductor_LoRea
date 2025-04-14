#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"

// ===<<< BEGIN EXCEPTION BLOCK >>>=== //
exception_block create_null_exception_block() {
    exception_block eb;
    eb.n = 0;
    eb.m = 0;
    eb.mapping = NULL;
    eb.mat = NULL;
    return eb;
}

exception_block create_empty_exception_block(unsigned n, unsigned m) {
    exception_block eb;
    eb.n = n;
    eb.m = m;
    eb.mapping = (int*)malloc(2 * m * sizeof(int));
    eb.mat = (int*)malloc(n * m * sizeof(int));

    if (!eb.mapping || !eb.mat) {
        fprintf(stderr, "Failed to allocate exception_block\n");
        free(eb.mapping);
        free(eb.mat);
        exit(EXIT_FAILURE);
    }
    return eb;
}

exception_block create_exception_block(unsigned n, unsigned m, int* mapping, int* mat) {
    exception_block eb = create_empty_exception_block(n,m);
    memcpy(eb.mapping, mapping, 2 * m * sizeof(int));
    memcpy(eb.mat, mat, n * m * sizeof(int));
    return eb;
}

void free_exception_block(exception_block* eb) {
    if (eb) {
        free(eb->mapping);
        free(eb->mat);
    }
}

void print_exception_block(exception_block* eb) {
    printf("Exception Block:\n");
    printf("mapping: [");
    for (unsigned i = 0; i < 2 * eb->m - 1; i++)
        printf("%d, ", eb->mapping[i]);
    printf("%d]\n", eb->mapping[2 * eb->m - 1]);

    printf("mat:\n");
    for (unsigned i = 0; i < eb->n; i++) {
        printf("[");
        for (unsigned j = 0; j < eb->m - 1; j++)
            printf("%d, ", eb->mat[i * eb->m + j]);
        printf("%d]\n", eb->mat[i * eb->m + eb->m - 1]);
    }
}
// ===<<< END EXCEPTION BLOCK >>>=== //

// ===<<< BEGIN MAIN TERM >>>=== //
main_term create_null_main_term() {
    main_term mt;
    mt.c = 0;
    mt.e = 0;
    mt.row = NULL;
    mt.exceptions = NULL;
    return mt;
}

main_term create_empty_main_term(unsigned c, unsigned e) {
    main_term mt;
    mt.c = c;
    mt.e = e;
    mt.row = (int*)malloc(c * sizeof(int));
    mt.exceptions = (exception_block*)malloc(e * sizeof(exception_block));

    if (!mt.row || !mt.exceptions) {
        fprintf(stderr, "Failed to allocate main_term\n");
        free(mt.row);
        free(mt.exceptions);
        exit(EXIT_FAILURE);
    }

    return mt;
}

main_term create_main_term(unsigned c, unsigned* row, unsigned e, exception_block* exceptions) {
    main_term mt = create_empty_main_term(c,e);

    memcpy(mt.row, row, c * sizeof(int));
    for (unsigned i = 0; i < e; i++) {
        mt.exceptions[i] = exceptions[i];
    }
    return mt;
}

void free_main_term(main_term* mt) {
    if (mt) {
        for (unsigned i = 0; i < mt->e; i++)
            free_exception_block(&mt->exceptions[i]);
        free(mt->exceptions);
        free(mt->row);
    }
}

void print_main_term(main_term* mt, int verbosity) {
    printf("Main Term:\n");
    printf("row: [");
    for (unsigned i = 0; i < mt->c - 1; i++)
        printf("%d, ", mt->row[i]);
    printf("%d]\n", mt->row[mt->c - 1]);

    printf("Exceptions: %u\n", mt->e);
    if (verbosity)
    {
        for (unsigned i = 0; i < mt->e; i++) {
            printf("\tException Block %u:\n", i);
            print_exception_block(&mt->exceptions[i]);
        }
    }
}
// ===<<< END MAIN TERM >>>=== //

// ===<<< BEGIN OPERAND BLOCK >>>=== //
operand_block create_null_operand_block() {
    operand_block ob;
    ob.r = 0;
    ob.c = 0;
    ob.terms = NULL;
    return ob;
}

operand_block create_empty_operand_block(unsigned r, unsigned c) {
    operand_block ob;
    ob.r = r;
    ob.c = c;
    ob.terms = (main_term*)malloc(r * sizeof(main_term));

    if (!ob.terms) {
        fprintf(stderr, "Failed to allocate operand_block\n");
        exit(EXIT_FAILURE);
    }
    return ob;
}

operand_block create_operand_block(unsigned r, unsigned c, main_term* terms) {
    operand_block ob = create_empty_operand_block(r,c);
    for (unsigned i = 0; i < r; i++) {
        ob.terms[i] = terms[i];
    }
    return ob;
}

void free_operand_block(operand_block* ob) {
    if (ob) {
        for (unsigned i = 0; i < ob->r; i++)
            free_main_term(&ob->terms[i]);
        free(ob->terms);
    }
}

void print_operand_block(operand_block* ob, int verbosity) {
    printf("Operand Block: %u terms, %u columns\n", ob->r, ob->c);
    if (verbosity)
    {
        for (unsigned i = 0; i < ob->r; i++) {
            printf("== Main Term %u ==\n", i);
            print_main_term(&ob->terms[i], verbosity - 1);
        }
    }
}
// ===<<< END OPERAND BLOCK >>>=== //

// ===<<< BEGIN MGU SCHEMA >>>=== //
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

mgu_schema* create_mgu_from_matrices(const matrix_schema* ms1, const matrix_schema* ms2) 
{
    unsigned n = 0;

    // Find the number of common elements
    for (unsigned i = 0; i < ms1->m; i++) {
        for (unsigned j = 0; j < ms2->m; j++) {
            if (ms1->columns[i] == ms2->columns[j]) {
                n++;
                break;
            }
        }
    }
    
    // Create MGU schema
    mgu_schema* mgu = (mgu_schema*)malloc(sizeof(mgu_schema));
    mgu->m = n;

    if (n == 0) printf("No common elements found between matrix schemas\n");
    else 
    {
        mgu->columns =   (unsigned*)malloc(n * sizeof(unsigned));
        mgu->mapping_L = (unsigned*)malloc(n * sizeof(unsigned));
        mgu->mapping_R = (unsigned*)malloc(n * sizeof(unsigned));

        if (!mgu->columns || !mgu->mapping_L || !mgu->mapping_R) {
            fprintf(stderr, "Unable to allocate memory inside mgu_schema\n");
            free_mgu_schema(mgu);
            exit(EXIT_FAILURE);
        }

        // Fill the mgu_schema
        unsigned index = 0;
        for (unsigned i = 0; i < ms1->m; i++) {
            for (unsigned j = 0; j < ms2->m; j++) {
                if (ms1->columns[i] == ms2->columns[j]) {
                    mgu->columns[index] = ms1->columns[i];
                    mgu->mapping_L[index] = ms1->mapping[i];
                    mgu->mapping_R[index] = ms2->mapping[j];
                    index++;
                    break;
                }
            }
        }
    }

    return mgu;
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
    printf("columns:   [");
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
// ===<<< END MGU SCHEMA >>>=== //

// ===<<< BEGIN MATRIX SCHEMA >>>=== //
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
// ===<<< END MATRIX SCHEMA >>>=== //

// ===<<< BEGIN L2, L3 >>>=== //
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
// ===<<< END L2, L3 >>>=== //


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
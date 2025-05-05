#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "structures.h"

// ===<<< BEGIN EXCEPTION BLOCK >>>=== //
exception_block create_null_exception_block() {
    exception_block eb;
    eb.n = 0;
    eb.m = 0;
    eb.ms  = NULL;
    eb.mat = NULL;
    return eb;
}

exception_block create_empty_exception_block(unsigned n, unsigned m) {
    exception_block eb;
    eb.n = n;
    eb.m = m;
    eb.ms = NULL;
    // printf("n,m: %u, %u\n",n,m); // Check
    eb.mat = (int*)malloc(n * m * sizeof(int));

    if (!eb.mat) {
        fprintf(stderr, "Failed to allocate exception_block\n");
        free(eb.mat);
        exit(EXIT_FAILURE);
    }
    return eb;
}

exception_block create_exception_block(unsigned n, unsigned m, mgu_schema *ms, int* mat) {
    exception_block eb = create_empty_exception_block(n,m);
    eb.ms = deep_copy_mgu_schema(ms);
    memcpy(eb.mat, mat, n * m * sizeof(int));
    return eb;
}

void free_exception_block(exception_block* eb) {
    if (eb) {
        free_mgu_schema(eb->ms);
        free(eb->mat);
    }
}

void print_exception_block(exception_block* eb, unsigned matrix_idx, unsigned exc_blk_idx) {
    printf("%% BEGIN: Exception subset%u.%u (%u,%u)\n",matrix_idx,exc_blk_idx,eb->n,eb->m);
    // print_mgu_schema(eb->ms); // Too much
    print_mgu_compact(eb->ms,eb->m*2);

    for (unsigned i = 0; i < eb->n; i++) {
        printf("Row %u: ",i);
        for (unsigned j = 0; j < eb->m - 1; j++)
            printf("%d,", eb->mat[i * eb->m + j]);
        printf("%d\n", eb->mat[i * eb->m + eb->m - 1]);
    }
    printf("%% END: Exception subset%u.%u\n",matrix_idx,exc_blk_idx);
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
    if (e!=0) mt.exceptions = (exception_block*)malloc(e * sizeof(exception_block));
    else mt.exceptions = NULL;
    
    if (((e!=0 && (!mt.row || !mt.exceptions))) || (e==0 && !mt.row)) {
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

void print_main_term(main_term* mt, unsigned matrix_idx, int verbosity) {
    // Print line
    printf("%u,",mt->e);
    for (unsigned i = 0; i < mt->c - 1; i++)
        printf("%d,", mt->row[i]);
    printf("%d\n", mt->row[mt->c - 1]);

    // Print exceptions
    if (verbosity)
    {
        for (unsigned i = 0; i < mt->e; i++) {
            print_exception_block(&mt->exceptions[i], matrix_idx, i+1);
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

void print_operand_block(operand_block* ob, unsigned matrix_idx, int verbosity) {
    printf("Operand Block: %u terms, %u columns\n", ob->r, ob->c);
    if (verbosity)
    {
        for (unsigned i = 0; i < ob->r; i++) {
            printf("== Main Term %u ==\n", i);
            print_main_term(&ob->terms[i], matrix_idx, verbosity - 1);
        }
    }
}
// ===<<< END OPERAND BLOCK >>>=== //

// ===<<< BEGIN RESULT BLOCK >>>=== //
result_block create_null_result_block() {
    result_block rb;
    rb.t1 = 0;
    rb.t2 = 0;
    rb.r1 = 0;
    rb.r2 = 0;
    rb.r = 0;
    rb.c1 = 0;
    rb.c2 = 0;
    rb.c = 0;
    rb.terms = NULL;
    rb.valid = NULL;
    rb.ms = NULL;
    return rb;
}

result_block create_empty_result_block(unsigned r1, unsigned r2, unsigned c1, unsigned c2, unsigned c, mgu_schema *ms) {
    result_block rb;
    rb.t1 = 0;
    rb.t2 = 0;
    rb.r1 = r1;
    rb.r2 = r2;
    rb.r = r1 * r2;
    rb.c1 = c1;
    rb.c2 = c2;
    rb.c = c;
    rb.ms = ms;

    rb.terms = (main_term*)malloc(rb.r * sizeof(main_term));
    rb.valid = (unsigned*)malloc(rb.r * sizeof(unsigned));

    if (!rb.terms || !rb.valid) {
        fprintf(stderr, "Failed to allocate result_block\n");
        exit(EXIT_FAILURE);
    }

    return rb;
}

result_block create_result_block(unsigned t1, unsigned t2, unsigned r1, unsigned r2, unsigned c1, unsigned c2, unsigned c, main_term* terms, unsigned* valid, mgu_schema *ms) {
    result_block rb = create_empty_result_block(r1, r2, c1, c2, c, ms);
    rb.t1 = t1;
    rb.t2 = t2;

    for (unsigned i = 0; i < rb.r; i++) {
        rb.terms[i] = terms[i];
        rb.valid[i] = valid[i];
    }

    return rb;
}

void free_result_block(result_block* rb) {
    if (rb) {
        for (unsigned i = 0; i < rb->r; i++)
            free_main_term(&rb->terms[i]);

        free(rb->terms);
        free(rb->valid);
        free_mgu_schema(rb->ms);
    }
}

void print_result_block(result_block* rb, int verbosity) {
    printf("Result Block: t1=%u, t2=%u | %u x %u terms (%u unified), c1=%u, c2=%u, c=%u\n",
           rb->t1, rb->t2, rb->r1, rb->r2, rb->r, rb->c1, rb->c2, rb->c);

    if (verbosity) {
        for (unsigned i = 0; i < rb->r; i++) {
            // printf("== Result Term [%u][%u] (valid=%u) ==\n", i / rb->r2, i % rb->r2, rb->valid[i]);
            if (rb->valid[i]==0) 
            {
                printf("Row %u-%u: ",i/rb->r2+1, i%rb->r2+1);
                print_main_term(&rb->terms[i], 3, verbosity - 1);
            }
            else if (rb->valid[i]==1) printf("Rows %u-%u subsumed by exception\n",i/rb->r2+1, i%rb->r2+1);
            else printf("Rows %u-%u not unifiable\n",i/rb->r2+1, i%rb->r2+1);
        }
    }
}
// ===<<< END RESULT BLOCK >>>=== //

// ===<<< BEGIN MGU SCHEMA >>>=== //
mgu_schema* create_empty_mgu_schema(const unsigned n_common, const unsigned n_uncommon_L, const unsigned n_uncommon_R) {
    mgu_schema *ms = malloc(sizeof(mgu_schema));
    if (!ms) {
        fprintf(stderr, "Unable to allocate memory for mgu_schema\n");
        exit(EXIT_FAILURE);
    }

    ms->n_common = n_common;
    ms->common_columns = (unsigned*)malloc(n_common * sizeof(unsigned));
    ms->common_L = (unsigned*)malloc(n_common * sizeof(unsigned));
    ms->common_R = (unsigned*)malloc(n_common * sizeof(unsigned));

    ms->n_uncommon_L = n_uncommon_L;
    ms->n_uncommon_R = n_uncommon_R;
    ms->uncommon_L = (unsigned*)malloc(2 * n_uncommon_L * sizeof(unsigned));
    ms->uncommon_R = (unsigned*)malloc(2 * n_uncommon_R * sizeof(unsigned));
    ms->new = 0; // ??
    ms->new_indices = NULL;

    if (!ms->common_columns || !ms->common_L || !ms->common_R || !ms->uncommon_L || !ms->uncommon_R) {
        fprintf(stderr, "Unable to allocate memory inside mgu_schema\n");
        free(ms->common_columns); free(ms->common_L); free(ms->common_R); 
        free(ms->uncommon_L); free(ms->uncommon_R);
        free(ms);
        exit(EXIT_FAILURE);
    }

    return ms;
}

// n must be the total number of elements, that is,n_pairs*2
// mapping must not have '_-_' entry, but not checked (maybe should)
mgu_schema* create_mgu_from_mapping(unsigned *mapping, const unsigned n, const unsigned n_L, const unsigned n_R) 
{
    unsigned i;
    unsigned n_common = 0;
    unsigned news = 0;

    for (i=0; i<n; i+=2)
    {
        // 0 value in mapping means '_'
        if (mapping[i] && mapping[i+1]) {n_common++;} 
        else if (!mapping[i] && !mapping[i+1]) {news++;}
    }

    mgu_schema *ms = create_empty_mgu_schema(n_common, n_L-n_common, n_R-n_common);
    ms->new_indices = (unsigned*)malloc(news*sizeof(unsigned));
    ms->new = news; 

    unsigned last_uncommon_L = 0;
    unsigned last_uncommon_R = 0;
    unsigned last_common = 0;
    unsigned last_new = 0;

    bool was_last_uncommon_L = false;
    bool was_last_uncommon_R = false;

    for (i=0; i<n; i+=2)
    {
        // Column taken from right main term, uncommon in R
        if (!mapping[i] && mapping[i+1]) // X-_ pattern
        {
            if (was_last_uncommon_R)
            {
                ms->uncommon_R[last_uncommon_R-1]++;
            }
            else
            {
                ms->uncommon_R[last_uncommon_R] = mapping[i+1];
                ms->uncommon_R[last_uncommon_R+1] = 1;
                last_uncommon_R += 2;
                was_last_uncommon_R = true;
                was_last_uncommon_L = false;
            }
        }
        // Column taken from left main term, uncommon in L
        else if (mapping[i] && !mapping[i+1]) // _-X pattern
        {
            if (was_last_uncommon_L)
            {
                ms->uncommon_L[last_uncommon_L-1]++;
            }
            else
            {
                ms->uncommon_L[last_uncommon_L] = mapping[i];
                ms->uncommon_L[last_uncommon_L+1] = 1;
                last_uncommon_L += 2;
                was_last_uncommon_R = false;
                was_last_uncommon_L = true;
            }
        }
        // Column in common
        else if (!mapping[i] && !mapping[i+1]) // _-_ pattern
        {
            ms->new_indices[last_new]=i;
            last_new++;
        }
        else // X-Y pattern
        {
            ms->common_columns[last_common] = (i/2) + 1;
            ms->common_L[last_common] = mapping[i];
            ms->common_R[last_common] = mapping[i+1];
            last_common++;
            was_last_uncommon_R = false;
            was_last_uncommon_L = false;
        }
    }
    return ms;
}

mgu_schema* deep_copy_mgu_schema(const mgu_schema* ms) {
    if (!ms) return NULL;

    mgu_schema* result = create_empty_mgu_schema(ms->n_common, ms->n_uncommon_L, ms->n_uncommon_R);

    memcpy(result->common_columns, ms->common_columns, ms->n_common * sizeof(unsigned));
    memcpy(result->common_L,       ms->common_L,       ms->n_common * sizeof(unsigned));
    memcpy(result->common_R,       ms->common_R,       ms->n_common * sizeof(unsigned));
    memcpy(result->uncommon_L,     ms->uncommon_L,     2 * ms->n_uncommon_L * sizeof(unsigned));
    memcpy(result->uncommon_R,     ms->uncommon_R,     2 * ms->n_uncommon_R * sizeof(unsigned));
    memcpy(result->new_indices,    ms->new_indices,    ms->new * sizeof(unsigned));

    return result;
}

void free_mgu_schema(mgu_schema* ms) {
    if (ms) {
        free(ms->common_columns);
        free(ms->common_L);
        free(ms->common_R);
        free(ms->uncommon_L);
        free(ms->uncommon_R);
        free(ms->new_indices);
        free(ms);
    }
}

void print_mgu_schema(mgu_schema* ms) {
    printf("common_columns: [");
    for (unsigned i = 0; i < ms->n_common - 1; i++) {
        printf("%u, ", ms->common_columns[i]);
    }
    if (ms->n_common > 0) printf("%u", ms->common_columns[ms->n_common - 1]);
    printf("]\n");

    printf("common_L: [");
    for (unsigned i = 0; i < ms->n_common - 1; i++) {
        printf("%u, ", ms->common_L[i]);
    }
    if (ms->n_common > 0) printf("%u", ms->common_L[ms->n_common - 1]);
    printf("]\n");

    printf("common_R: [");
    for (unsigned i = 0; i < ms->n_common - 1; i++) {
        printf("%u, ", ms->common_R[i]);
    }
    if (ms->n_common > 0) printf("%u", ms->common_R[ms->n_common - 1]);
    printf("]\n");

    printf("uncommon_L: [");
    for (unsigned i = 0; i < 2 * ms->n_uncommon_L; i += 2) {
        printf("(%u, %u)", ms->uncommon_L[i], ms->uncommon_L[i + 1]);
        if (i + 2 < 2 * ms->n_uncommon_L) printf(", ");
    }
    printf("]\n");

    printf("uncommon_R: [");
    for (unsigned i = 0; i < 2 * ms->n_uncommon_R; i += 2) {
        printf("(%u, %u)", ms->uncommon_R[i], ms->uncommon_R[i + 1]);
        if (i + 2 < 2 * ms->n_uncommon_R) printf(", ");
    }
    printf("]\n");

    printf("%u new_indices: [",ms->new);
    for (unsigned i = 0; i < ms->new; i++) {
        if (i==(ms->new-1)) printf("%u", ms->new_indices[i]);
        else printf("%u, ", ms->new_indices[i]);
    }
    printf("]\n");
}

void print_mgu_compact(mgu_schema *ms, unsigned total_columns) {
    // Create a flat mapping array
    unsigned *mapping = calloc(total_columns, sizeof(unsigned));
    if (!mapping) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    
    // print_mgu_schema(ms); // Check

    // Fill common entries // X-Y schema
    for (unsigned i = 0; i < ms->n_common; i++) {
        unsigned idx = ms->common_columns[i] - 1;
        mapping[2 * idx]     = ms->common_L[i];
        mapping[2 * idx + 1] = ms->common_R[i];
    }

    // Fill uncommon_L blocks // X-_ schema
    unsigned pos = 0;
    for (unsigned i = 0; i < ms->n_uncommon_L; i++) {
        unsigned start = ms->uncommon_L[2 * i];
        unsigned length = ms->uncommon_L[2 * i + 1];
        for (unsigned j = 0; j < length; j++) {
            // Find next empty slot (i.e., both values are zero)
            while (mapping[2 * pos] || mapping[2 * pos + 1]) pos++;
            mapping[2 * pos] = start + j;
            // mapping[2 * pos + 1] = 0; // Already zero
            pos++;
        }
    }

    // Fill uncommon_R blocks // _-Y schema
    pos = 0;
    for (unsigned i = 0; i < ms->n_uncommon_R; i++) {
        unsigned start = ms->uncommon_R[2 * i];
        unsigned length = ms->uncommon_R[2 * i + 1];
        for (unsigned j = 0; j < length; j++) {
            while (mapping[2 * pos] || mapping[2 * pos + 1]) pos++;
            // mapping[2 * pos] = 0; // Already zero
            mapping[2 * pos + 1] = start + j;
            pos++;
        }
    }

    // Fill new columns // _-_ schema
    pos = 0;
    for (unsigned i = 0; i < ms->new; i++) {
        unsigned idx = ms->new_indices[i] - 1;
        mapping[2 * idx]     = 0;
        mapping[2 * idx + 1] = 0;
    }

    // Print reconstructed mapping
    for (unsigned i = 0; i < total_columns/2; i++) {
        unsigned left = mapping[2 * i];
        unsigned right = mapping[2 * i + 1];
        if (left == 0) printf("_");
        else printf("%u", left);
        printf("-");
        if (right == 0) printf("_");
        else printf("%u", right);
        if (i + 1 < total_columns/2) printf(",");
    }
    printf("\n");

    free(mapping);
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
#include <stdio.h>

/* Structs */
typedef struct L2 L2;
typedef struct L3 L3;


struct L2 {
    int ind, count, by;
    L3 *head, *tail;  
};

struct L3 {
    int ind;
    L3 *next;  
};

typedef struct {
    unsigned m;
    unsigned *columns;
    unsigned *mapping;
} matrix_schema;

typedef struct {
    unsigned m;
    unsigned *columns;
    unsigned *mapping_L;
    unsigned *mapping_R;
} mgu_schema;


/* Functions */

mgu_schema* create_empty_mgu_schema(const unsigned m);
mgu_schema* create_mgu_schema(const unsigned m, unsigned* columns, unsigned* mappings_L, unsigned* mappings_R);
mgu_schema* create_mgu_from_matrices(const matrix_schema* ms1, const matrix_schema* ms2);
void free_mgu_schema(mgu_schema* ms);
void print_mgu_schema(mgu_schema* ms) ;
matrix_schema* create_empty_matrix_schema(const unsigned m);
matrix_schema* create_matrix_schema(const unsigned m, unsigned* columns, unsigned* mappings);
void free_matrix_schema(matrix_schema* ms);
void print_matrix_schema(matrix_schema* ms);
L2 create_L2_empty();
L2 create_L2(int ind_in, int count_in, int by_in, L3* head_in, L3* tail_in);
L3* create_L3_empty();
L3* create_L3(int ind_in, L3* next_in);
void free_L2(L2 node);
void free_L3(L3* node);
L2 copy_L2(L2 node);
L3* copy_L3(L3* node);
int timespec_subtract (struct timespec *result, struct timespec *x, struct timespec *y);
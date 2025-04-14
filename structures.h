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

typedef struct {
    unsigned n;   // Number of exceptions within the exception block
    unsigned m;   // Number of columns for all exceptions within the exception block
    int *mapping; // [2*m] array with mapping between columns in the exception block to columns in main term block, -1 for new columns
    int *mat;     // 1D matrix of size n*m containing the exceptions
} exception_block;

typedef struct {
    unsigned c;                  // Number of columns in the main term
    int *row;                    // 1D array containing the values of the main term
    unsigned e;                  // Number of exception blocks for the main term
    exception_block *exceptions; // Array with e exception blocks for the main term
} main_term;

typedef struct {
    unsigned r;       // Number of main terms
    unsigned c;       // Number of columns for all main terms within the block
    main_term *terms; // r main_term array, represents a block within an operand matrix
} operand_block;

/* Functions */
exception_block create_null_exception_block();
exception_block create_empty_exception_block(unsigned n, unsigned m);
exception_block create_exception_block(unsigned n, unsigned m, int* mapping, int* mat);
void free_exception_block(exception_block* eb);
void print_exception_block(exception_block* eb);

main_term create_null_main_term();
main_term create_empty_main_term(unsigned c, unsigned e);
main_term create_main_term(unsigned c, unsigned* row, unsigned e, exception_block* exceptions);
void free_main_term(main_term* mt);
void print_main_term(main_term* mt, int verbosity);

operand_block create_null_operand_block();
operand_block create_empty_operand_block(unsigned r, unsigned c);
operand_block create_operand_block(unsigned r, unsigned c, main_term* terms);
void free_operand_block(operand_block* ob);
void print_operand_block(operand_block* ob, int verbosity);

mgu_schema* create_empty_mgu_schema(const unsigned m);
mgu_schema* create_mgu_schema(const unsigned m, unsigned* columns, unsigned* mappings_L, unsigned* mappings_R);
mgu_schema* create_mgu_from_matrices(const matrix_schema* ms1, const matrix_schema* ms2);
void free_mgu_schema(mgu_schema* ms);
void print_mgu_schema(mgu_schema* ms);

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
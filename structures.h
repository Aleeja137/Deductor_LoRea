#include <stdio.h>

/* Structs */
typedef struct L1 L1;
typedef struct L2 L2;
typedef struct L3 L3;

struct L1 {
    int val;
    L2 *info;  
    char *name;
};

struct L2 {
    int val;
    L3 *head;  
    L3 *tail;
    L1 *me;    
};

struct L3 {
    L1 *var;
    L3 *next;  
};

/* Functions */
L1* create_L1_empty();
L1* create_L1(int val_in, L2 *info_in, char *name_in);
// L1* create_L1(int val_in, L2* info_in);
L2* create_L2_empty();
L2* create_L2(int val_in, L3* head_in, L3* tail_in, L1* me_in);
L3* create_L3_empty();
L3* create_L3(L1* var_in, L3* next_in);
L1** constr_mat_vacia(size_t n, size_t m);
void free_L1(L1* node);
void free_L2(L2* node);
void free_L3(L3* node);
void free_L1_mat(int n, int m, L1** mat);
L1* copy_L1(L1* node);
L2* copy_L2(L2* node);
L3* copy_L3(L3* node);
L1** copy_mat(size_t n, size_t m, L1** mat);
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

/* Functions */
L2 create_L2_empty();
L2 create_L2(int ind_in, int count_in, int by_in, L3* head_in, L3* tail_in);
L3* create_L3_empty();
L3* create_L3(int ind_in, L3* next_in);
void free_L2(L2 node);
void free_L3(L3* node);
L2 copy_L2(L2 node);
L3* copy_L3(L3* node);
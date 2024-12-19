#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#include "dictionary.h"
#include "structures.h"

// #define N 50
// #define M 10
#define N 3
#define M 6
#define minus_zero 555 

struct nlist *dict;


void read_matrix_dimensions(FILE *stream, int *n, int *m) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *endptr, *e;
    long int num;

    while ((read = getline(&line, &len, stream)) != -1) {
        if (strstr(line, "BEGIN") != NULL) {
            e = strchr(line, '(');
            if (!e) break;

            num = strtol(e + 1, &endptr, 10);
            if (endptr == e + 1) {
                fprintf(stderr, "Could not read matrix dimensions\n");
                exit(EXIT_FAILURE);
            }
            *n = num;

            e = strchr(endptr, ',');
            if (!e) break;

            num = strtol(e + 1, &endptr, 10);
            if (endptr == e + 1) {
                fprintf(stderr, "Could not read matrix dimensions\n");
                exit(EXIT_FAILURE);
            }
            *m = num;
            break;
        }
    }
    free(line);
}

void read_matrix(FILE *stream, int **matrix, int n, int m) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int row = 0, col, line_len = 1 + m + 1 + (m * 2) + 2;
    *matrix = (int *)malloc(n * line_len * sizeof(int));

    while ((read = getline(&line, &len, stream)) != -1 && row < n) {
        if (strstr(line, "END") != NULL)
            break;

        (*matrix)[row * line_len] = m;               // Number of elements in the line
        (*matrix)[row * line_len + line_len - 2] = row; // Row index

        col = 1; // Start indexing columns from 1
        char *tok = strtok(line, ",");
        while (tok) {
            int index = (row * line_len) + col;
            if (isdigit(tok[0])) {
                (*matrix)[index] = atoi(tok);
            } else {
                if (lookup(tok) == NULL) {
                    (*matrix)[index] = 0;
                    install(tok, col);
                } else {
                    (*matrix)[index] = -(lookup(tok)->defn);
                }
            }
            tok = strtok(NULL, ",\n");
            col++;
        }
        clear(); // Reset dictionary
        row++;
    }
    free(line);
}

void read_mat_file(char input_file[], int **mat1, int **mat2, int *n1, int *n2, int *m1, int *m2) {
    FILE *stream = fopen(input_file, "r");
    if (!stream) {
        fprintf(stderr, "Error opening file %s, exiting\n", input_file);
        exit(EXIT_FAILURE);
    }

    // Read first matrix
    read_matrix_dimensions(stream, n1, m1);
    read_matrix(stream, mat1, *n1, *m1);

    // Read second matrix
    read_matrix_dimensions(stream, n2, m2);
    read_matrix(stream, mat2, *n2, *m2);

    fclose(stream);
}


// Prints the matrix elements, not the metadata nor the unifiers
void print_mat_values(int *mat, int n, int m){
    int i, j;
    int start_elem = 1;
    // int start_unif = 1+m;
    int line_len = 1+m+1+(m*2)+2;
    
	for (i = 0; i < n; i++)
	{
		printf("[");
		for (j = 0; j < m; j++)
			printf("%d ", mat[i*line_len+start_elem+j]);
		printf("]\n");
	}
}

// Prints metadata from a matrix, usefull for testing the unifier in early stages
void print_mat_metadata(int *mat, int n, int m){
    int i, j;
    // int start_elem = 1;
    int start_unif = 1+m;
    int line_len = 1+m+1+(m*2)+2;
    
	for (i = 0; i < n; i++)
	{
		printf("[");
        printf("m: %d, nelem : %d, unifier: { ",mat[i*line_len],mat[i*line_len+start_unif]);
		for (j = 0; j < mat[i*line_len+start_unif]; j+=2)
			printf("%d<-%d ", mat[i*line_len+start_unif+1+j],mat[i*line_len+start_unif+1+j+1]);
		printf("}, rowA: %d, rowB: %d ]\n",mat[i*line_len+line_len-2],mat[i*line_len+line_len-1]);
	}
}

// Prints the unifier
void print_unifier(int *unifier, int m){
    int i;
    int n_elem = unifier[0]*2;
    int rowA = unifier[1+2*m];
    int rowB = unifier[1+2*m+1];
    
    printf("%d elements: [",n_elem);
	for (i = 0; i < n_elem; i+=2)
	{
        printf("%d<-%d ", unifier[1+i],unifier[1+i+1]);
	}
    printf("],\t\t\trowA: %d, rowB: %d\n",rowA+1, rowB+1);
}

void print_unifier_list(int *unifiers, int unif_count, int m){

    int i, unifier_size = 1+(2*m)+2;
    for (i=0; i<unif_count; i++)
    {
        print_unifier(&unifiers[i*unifier_size],m);
    }
    printf("Number of unifiers: %d\n",unif_count);
}
// Unify two elements from different rows
int unifier_a_b(int *row_a, int indexA, int *row_b, int indexB, int *unifier, int indexUnifier){

    // Make sure both rows have same number of elements
    int m = row_a[0];
    assert(row_a[0]==row_b[0]); 

    // Get elements (+m is added to second row, might be reversed from recursive calls, so need to check)
    int a, b, real_indexA = indexA, real_indexB = indexB;
    if (indexA > m) real_indexA = real_indexA-m; 
    if (indexB > m) real_indexB = real_indexB-m;
    a = row_a[real_indexA];
    b = row_b[real_indexB];

    // A is constant (or variable initialized to constant)
	if (a>0 && b > 0 && a!=b) // B is constant too
		return -1;
	else if (a>0 && b <= 0)        // B is variable
		return unifier_a_b(row_b,indexB,row_a,indexA,unifier,indexUnifier);

	// A is variable
	if (a <= 0) // B is constant
	{
        // Update unifier (a<-b)
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB; 
	}

	return 0;
}

// Unifier must be pointing to row_a's unifier
int unifier_rows(int *row_a, int *row_b, int *unifier){
    int i, result;
    assert(row_a[0]==row_b[0]);
    int m = row_a[0];

	for (i=1; i<=m; i++)
	{
        unifier[0] = 0;
        // The unifier function handles the entire row including metadata, so pointers do not need to be modified
		result = unifier_a_b(row_a, i, row_b, i+m, unifier, 2*(i-1));
		if (result != 0) return result;
	}

	return 0;
}

// Correct/reduce/verify the unifier, unifier pointer must be pointing to row_a's unifier
int correct_unifier(int *row_a, int *row_b, int *unifier){
    int m = row_a[0];
    assert(row_a[0]==row_b[0]);
    
    int i, n_substitutions = 0;
    L2 *lst = (L2*) malloc (2*m*sizeof(L2));
    for (i=0;i<2*m;i++){
        lst[i] = create_L2_empty();
    }

    // For each element pair, get indexes and perform (x<-y)
    for (i=0; i<2*m; i+=2)
    {
        int x = unifier[1+i]; 
        int y = unifier[1+i+1];
        int val_y;
        int val_x; 

        if (x == y && x == 0) continue; // Empty case

        // If any of the two elements is a repeated variable, get read index
        if (x <= m && row_a[x] < 0)
            x = -row_a[x];
        else if (x > m && row_b[x-m] < 0) 
            x = -row_b[x-m]+m;
        if (y <= m && row_a[y] < 0) 
            y = -row_a[y];
        else if (y > m && row_b[y-m] < 0)
            y = -row_b[y-m]+m; 

        // If any of them was substituted before, get the corresponding elements
        if (lst[x-1].count > 0) x = lst[x-1].by;
        if (lst[y-1].count > 0) y = lst[y-1].by;
        if (x==y) continue; 
        // No need to get real index, the substitution is done for the real index

        // Get the value of x and y
        if (y > m) val_y = row_b[y-m];
        else val_y = row_a[y];
        if (x > m) val_x = row_b[x-m];
        else val_x = row_a[x];
        // If both constants 
        if (val_x > 0 && val_y > 0 && val_x!=val_y) return -1;
        else if (val_x > 0 && val_y > 0) continue;

        if (val_x > 0 && val_y == 0) // (y<-x)
        {   
            // make the replacement on y
            lst[y-1].count = 1;
            lst[y-1].by    = x;
            lst[y-1].ind   = y;

            // Update all variables replaced by y to be replaced by x
            L3 *current = lst[y-1].head;
            while (current != NULL)
            {
                lst[current->ind-1].by = x;
                current = current->next;
            }

            // Add the replacement list of y, and y itself, to replacement list of x
            if (lst[x-1].head)
            {
                lst[x-1].tail->next = create_L3(y,lst[y-1].head);
                if (lst[y-1].head) lst[x-1].tail = lst[y-1].tail;
            }
            else
            {
                lst[x-1].head = create_L3(y,lst[y-1].head);
                lst[x-1].tail = lst[x-1].head;
                if (lst[y-1].head) lst[x-1].tail = lst[y-1].tail;
            } 
            lst[y-1].head = lst[y-1].tail = NULL;
        }
        else // (x<-y)
        {
            // make the replacement on x
            lst[x-1].count = 1;
            lst[x-1].by    = y;
            lst[x-1].ind   = x;

            // Update all variables replaced by y to be replaced by x
            L3 *current = lst[x-1].head;
            while (current != NULL)
            {
                int current_index = current->ind;
                // printf("Altering by of index %d\n",current_index);
                lst[current_index-1].by = y;
                current = current->next;
            }

            // Add the replacement list of y, and y itself, to replacement list of x
            if (lst[y-1].head)
            {
                lst[y-1].tail->next = create_L3(x,lst[x-1].head);
                lst[y-1].tail = lst[y-1].tail->next;
                if (lst[x-1].head) lst[y-1].tail = lst[x-1].tail;

            }
            else
            {
                lst[y-1].head = create_L3(x,lst[x-1].head);
                lst[y-1].tail = lst[y-1].head;
                if (lst[x-1].head) lst[y-1].tail = lst[x-1].tail;
            }
            lst[x-1].head = lst[x-1].tail = NULL;
        }
    }

    int last_unifier = 0;
    // For each element, add the substitutions to the unifier
    for (i=0; i<2*m; i++)
    {
        int y = i;
        int x;
        if (lst[y].count == 0 && lst[y].head)
        {
            // Get substitutions (x <- y)
            L3* current = lst[y].head;
            while (current != NULL)
            {
                x = current->ind;
                unifier[1+last_unifier]   = x;
                unifier[1+last_unifier+1] = y+1;
                current = current->next;
                n_substitutions++;
                last_unifier+=2;
            }
        }
    }
    unifier[0] = n_substitutions;
    return 0;
}

// Return the unifiers for two given matrices. Must have same width, and unifiers must be initialized to n0*n1
int unifier_matrices(int *mat0, int *mat1, int n0, int n1, int *unifiers){

    int i, j, m, row_size, unifier_size, code, last_unifier;
    int *unifier;

    m = mat0[0];
    last_unifier = 0;
    row_size = 1+m+1+(2*m)+2;
    unifier_size = 1+(2*m)+2;

    for (i=0; i<n0; i++)
    {
        for (j=0; j<n1; j++)
        {
            unifier = &mat0[row_size*i + 1 + m]; 
            memset(unifier,0,unifier_size*sizeof(int));  // Revise full: This should be unnecessary, but it isn't, check why (not urgent)
            code = unifier_rows(&mat0[row_size * i], &mat1[row_size * j], unifier);
            if (code != 0) continue; // Rows cannot be unified

            code = correct_unifier(&mat0[row_size * i], &mat1[row_size * j], unifier);
            if (code != 0) continue; // Rows cannot be unified
            
            unifier[1+(2*m)]   = i;
            unifier[1+(2*m)+1] = j;
            memcpy(&unifiers[last_unifier*unifier_size],unifier,unifier_size*sizeof(int));
            last_unifier++;
        }
    }

    // Free the extra space
    unifiers = realloc(unifiers,last_unifier*unifier_size*sizeof(int));

    return last_unifier;
}

// void read_unif_mat_file();
// void process_files();

int main(int argc, char *argv[])
{
    int verbose = 0;
    char *csv_file = "benchmark/Set1_changed/test01.csv";

    if (argc>1) csv_file = argv[1];
    if (argc>2) verbose = 1;

    int *mat0=NULL, *mat1=NULL, n0,n1,m0,m1;

    read_mat_file(csv_file, &mat0, &mat1, &n0,&n1,&m0,&m1);
	printf("read_mat_file completed :)\n");
    printf("Dimensions for M1 are (%d,%d) and for M2 are (%d,%d)\n",n0,m0,n1,m1);

    if (verbose)
    {
        printf("\nValues and metadata for M1 from %s\n",csv_file);
        print_mat_values(mat0,n0,m0);

        printf("\nValues and metadata for M2 from %s\n",csv_file);
        print_mat_values(mat1,n1,m1);
    }

    // ----- test all matrix ----- //
	int *unifiers = NULL, unifier_size = 1+(2*m0)+2;
    unifiers = (int*) malloc (n0*n1*unifier_size*sizeof(int));
    int unif_count = unifier_matrices(mat0, mat1, n0, n1, unifiers);
    if (verbose) print_unifier_list(unifiers,unif_count,m0);
    else printf("Number of unifiers: %d\n",unif_count);
    // ----- test all matrix ----- //


    // ----- test two rows ----- //
    // int *unifier, code;
    // int row_size = 1+m0+1+(2*m0)+2, unifier_size = 1+(2*m0)+2;
    // unifier = &mat0[row_size*15 + 1 + m0]; 
    // memset(unifier,0,unifier_size*sizeof(int)); 
    // code = unifier_rows(&mat0[row_size * 14], &mat1[row_size * 7], unifier);
    // if (code != 0) printf("Not unifiable at STEP 1\n");
    // code = correct_unifier(&mat0[row_size * 14], &mat1[row_size * 7], unifier);
    // if (code != 0) printf("Not unifiable at STEP 2\n");
    // unifier[1+(2*m0)]   = 14;
    // unifier[1+(2*m0)+1] = 7;
    // print_unifier(unifier,m0);
    // ----- test two rows ----- //

    free(mat0);
    free(mat1);
	return 0;
}
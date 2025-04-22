#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#include "dictionary.h"
#include "structures.h"
// #include "AGT_hash.h"
#include "COM_hash.h"

#define ROW_STR_SIZE (snprintf(NULL, 0, "%d", INT_MAX) + 1)
Dictionary *const_dict;
Dictionary *var_dict;
Dictionary *unif_dict;
// bool chivato = false; // Check

struct nlist *dict;
int verbose = 0;

// read_mat_row is necessary to prevent 'tok' from two matrices taking the same 'tok+row_str' 
// but row still needs to start from 0 each read_matrix call, so additional variable
int read_mat_row = 0; 
int last_int = 1;

// #define malloc(X) my_malloc( X, __FILE__, __LINE__, __FUNCTION__)

// void* my_malloc(size_t size, const char *file, int line, const char *func)
// {

//     void *p = malloc(size);
//     printf ("Allocated = %s, %i, %s, %p[%li]\n", file, line, func, p, size);

//     /*Link List functionality goes in here*/

//     return p;
// }

// --------------------- UTILS START --------------------- //
void timespec_add(struct timespec *result, const struct timespec *t1, const struct timespec *t2) {
    result->tv_sec = t1->tv_sec + t2->tv_sec;
    result->tv_nsec = t1->tv_nsec + t2->tv_nsec;
    if (result->tv_nsec >= 1000000000L) {
        result->tv_sec++;
        result->tv_nsec -= 1000000000L;
    }
}

// Prints the matrix elements, not the metadata nor the unifiers
void print_mat_values(int *mat, int n, int m){
    int i, j;
    int line_len = m;
    
	for (i = 0; i < n; i++)
	{
		printf("[");
		for (j = 0; j < m; j++)
			printf("%d ", mat[i*line_len+j]);
		printf("]\n");
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

void print_mat_line(int *row, int m){
    int j;
    printf("[");
    for (j = 0; j < m; j++)
        printf("%d ", row[j]);
    printf("]\n");
}

int compare_mgus(int *my_mgu, int *other_mgu, int n, int m){
    int same = 1;
    int n_elems = n*m;
    int i;

    for (i = 0; i < n_elems; i++)
    {
        if (my_mgu[i] != other_mgu[i])
        {
            int row = i/m;
            int col = i%m;
            printf("\t"); print_mat_line(&my_mgu[row*m],m);
            printf("\t"); print_mat_line(&other_mgu[row*m],m);
            printf("\tDifferent elements at row %d col %d: %d != %d\n",row,col,my_mgu[i],other_mgu[i]);
            return 0;
        }
    }
    return same; 
}
// ---------------------- UTILS END ---------------------- //


// --------------------- READING FILE START --------------------- //
int read_num_blocks(FILE *stream, unsigned *s) {
    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, stream) == -1) return 0; // Failed to read the line
    if (strstr(line, "BEGIN") == NULL) {free(line); return 0;} // Not the correct line

    char *endptr;
    char *e = strchr(line, '(');
    if (!e) {free(line); return 0;} // Not the correct line
    int num = strtol(e + 1, &endptr, 10);
    if (endptr == e + 1) {free(line); return 0;}

    *s = (unsigned)num;
    free(line);
    return 0;
}

matrix_schema* read_matrix_schema_from_csv(const char* line, int m) {

    unsigned* columns = malloc(m * sizeof(unsigned));
    unsigned* mapping = malloc(m * sizeof(unsigned));

    // Parse the line
    char* line_copy = strdup(line);
    char* tok = strtok(line_copy, ",");
    int i = 0;
    while (tok && i < m) {
        columns[i] = (unsigned)atoi(tok);
        mapping[i] = (unsigned)i;
        tok = strtok(NULL, ",");
        i++;
        // printf("tok: %s,",tok); // Check
    }
    free(line_copy);

    if (i != m) {
        fprintf(stderr, "Mismatch in number of parsed columns\n");
        free(columns);
        free(mapping);
        exit(EXIT_FAILURE);
    }

    matrix_schema* ms = create_matrix_schema(m, columns, mapping);
    free(columns);
    free(mapping);
    return ms;
}

void read_dimensions(FILE *stream, unsigned *n, unsigned *m) {
    char *line = NULL;
    size_t len = 0;
    char *endptr, *e;
    long int num;

    getline(&line, &len, stream);
    // printf("Line in read_dimensions is: %s\n",line); // Check
    if (strstr(line, "BEGIN") == NULL) {
        fprintf(stderr, "Could not read matrix dimensions\n");
        exit(EXIT_FAILURE);
    }
    
    e = strchr(line, '(');
    if (!e) {
        fprintf(stderr, "Could not read matrix dimensions\n");
        exit(EXIT_FAILURE);
    };

    num = strtol(e + 1, &endptr, 10);
    if (endptr == e + 1) {
        fprintf(stderr, "Could not read matrix dimensions\n");
        exit(EXIT_FAILURE);
    }

    *n = num;

    e = strchr(endptr, ',');
    if (!e) {
        fprintf(stderr, "Could not read matrix dimensions\n");
        exit(EXIT_FAILURE);
    }

    num = strtol(e + 1, &endptr, 10);
    if (endptr == e + 1) {
        fprintf(stderr, "Could not read matrix dimensions\n");
        exit(EXIT_FAILURE);
    }
    *m = num;
    free(line);
}

void read_line(char *line, int *row, bool skip_first) {
    // printf("Line is '%s'\n",line); // Check
    char *tok = strtok(line, ",");
    if (skip_first) tok = strtok(NULL, ",\n");

    int col = 1;
    clear(var_dict);
    while (tok) {
        // printf("'%s' - ",tok); // Check
        if (!isupper(tok[0])) { // If no uppercase appears, it is a constant
            // printf("suspected constant tok: %s\n",tok); // Check
            const struct Symbol* s = get_value(tok, strlen(tok));
            // printf("CONSTANT VALUE: %d\n",s->value); // Check
            row[col-1] = s->value;
        } else { // It is a variable
            if (lookup(var_dict, tok) == NULL) {
                row[col-1] = 0;
                install(var_dict, tok, col);
                // if (verbose) printf("VARIABLE read element '%s' first appearance, assigned column %d\n", tok, col); // Check
            } else {
                row[col-1] = -(lookup(var_dict, tok)->defn);
                // if (verbose) printf("VARIABLE read element '%s' NOT first appearance, assigned index %d\n", tok, (*matrix)[index]); // Check
            }
        }
        tok = strtok(NULL, ",\n");
        col++;
    }
}

void read_exception_blocks(FILE *stream, main_term *mt, const bool result) {
    unsigned n, m;
    unsigned e = mt->e;
    exception_block *eb;
    char *line = NULL;
    size_t len = 0;
    // printf("Number of exceptions is %u\n",e); // Check
    for (unsigned i = 0; i < e; i++)
    {
        // Read dimensions of exception block (subset)
        read_dimensions(stream,&n,&m);
        // printf("n: %u, m: %u\n",n,m); // Check

        // Create empty exception block to be populated later
        mt->exceptions[i] = create_empty_exception_block(n,m);
        eb = &(mt->exceptions[i]);

        // Skip unflatened schema
        getline(&line, &len, stream);

        // Read mapping (Skip for now)
        getline(&line, &len, stream);
        // read_mapping(stream, eb->ms, m);

        // Skip flatened schema if reading from operand
        if (!result) getline(&line, &len, stream);

        for (size_t j = 0; j < n; j++)
        {            
            getline(&line, &len, stream);
            char *line_ptr = line;  
            // printf("Line before moving past Row: %s\n",line); // Check
            if (result) line_ptr = strchr(line_ptr, ':') + 2;
            // printf("Line after moving past Row:  %s\n\n",line_ptr); // Check
            read_line(line_ptr, &eb->mat[j*m], false);
            // Skip unifier if reading from result file
            if (result) getline(&line, &len, stream);
        }

        // Skip end exception subset line
        getline(&line, &len, stream);
    }
    
    free(line);
}

void read_operand_matrix(FILE *stream, operand_block *ob) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned row = 0;
    // unsigned col;

    // Skip unflatened schema
    getline(&line, &len, stream);

    // Skip flatened schema
    getline(&line, &len, stream);

    // Iterate the main term rows
    while ((read = getline(&line, &len, stream)) != -1 && row < ob->r) {
        
        // If end of matrix reached, exit
        if (strstr(line, "END") != NULL || strstr(line, "End") != NULL)
            break;
        
        // Get first token, which tells the number of exception blocks
        // printf("Line is before tok '%s'\n",line); // Check
        char *tok = strtok(strdup(line), ",");
        // printf("Line is after tok '%s'\n",line); // Check
        unsigned e = (unsigned)strtoul(tok, NULL, 10);
        // printf("The main term %u has %u exception blocks\n",row,e); // Check
        
        // Initialize the exception blocks
        ob->terms[row] = create_empty_main_term(ob->c,e);

        // Get a pointer to the main term for easier working
        main_term *mt = &(ob->terms[row]);

        // Read the rest of the line
        // printf("Line: %s\n",line); // Check
        read_line(line, mt->row, true);
        // print_mat_line(mt->row,mt->c); // Check

        // Read one by one the exception blocks
        if (e) read_exception_blocks(stream, mt, false);

        // Increment the row by 1
        row++;
    }

    free(line);
}

void get_mapping(char *line, unsigned n_pairs, unsigned *mapping){
    char *tok = strtok(line, " ,\n");
    unsigned count = 0;

    while (tok != NULL && count < n_pairs * 2) {
        char *dash = strchr(tok, '-');
        if (dash) {
            *dash = '\0'; 
            char *first = tok;
            char *second = dash + 1;

            mapping[count++] = (strcmp(first, "_") == 0)  ? 0 : (unsigned)atoi(first);
            mapping[count++] = (strcmp(second, "_") == 0) ? 0 : (unsigned)atoi(second);
        }
        tok = strtok(NULL, " ,\n");
    }
}

void read_result_matrix(FILE *stream, result_block *rb) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned row = 0;

    // Skip matrix header // TODO: Change the logic to do this elsewhere
    getline(&line, &len, stream);

    // Read block header
    getline(&line, &len, stream);
    // printf("line to be matched is: %s\n",line); // Check
    int matched = sscanf(line, "%% BEGIN: Matrix subset %u-%u (%u-%u,%u-%u,%u)",
                         &rb->t1, &rb->t2, &rb->r1, &rb->r2, &rb->c1, &rb->c2, &rb->c);

    if (matched != 7) {
        fprintf(stderr, "Could not read matrix subset info, matched: %d\n",matched);
        free_result_block(rb);
        free(line);
        exit(EXIT_FAILURE);
    }

    // Info for all possible main terms from the unification of M1 block (r1) and M2 block (r2)
    rb->r = rb->r1*rb->r2;
    rb->terms = (main_term*)malloc(rb->r * sizeof(main_term));
    rb->valid = (unsigned*) malloc(rb->r * sizeof(unsigned));
    for (size_t _ = 0; _ < rb->r; _++)
    {
        rb->valid[_] = 0;
    }
    

    // Skip largest schema
    getline(&line, &len, stream);

    // Get mapping info
    getline(&line, &len, stream);
    unsigned *mapping = (unsigned*)malloc(rb->c*2*sizeof(unsigned));
    get_mapping(line, rb->c, mapping);
    rb->ms = create_mgu_from_mapping(mapping, rb->c, rb->c1, rb->c2);

    // Skip flattened schema
    getline(&line, &len, stream);

    // Iterate the main term rows
    while ((read = getline(&line, &len, stream)) != -1 && row < rb->r) {
        
        // If end of matrix reached, exit
        if (strstr(line, "END") != NULL || strstr(line, "End") != NULL)
            break;
        
        // Get line indexes from row

        // check if line is unifiable, subsumed or not unifiable
        if (strncmp(line, "Rows ", 5) == 0) {
            if (strstr(line, "subsumed by exception")) {
                rb->valid[row] = 1;
                // printf("Reading line %u is subsumed by exception\n",row); // Check
            } else if (strstr(line, "not unifiable")) {
                rb->valid[row] = 2;
                // printf("Reading line %u is not unifiable\n",row); // Check
            }
            // rb->terms[row] = NULL; // This not necessary I would say
            row++;
            continue;
        }

        // If unifiable, read number of exception blocks
        unsigned d1, d2, e;
        // printf("Line is: %s\n",line); // Check
        if (sscanf(line, "Row %u-%u: %u", &d1, &d2, &e) != 3) 
        {
            fprintf(stderr, "Could not read number of exception blocks in row\n");
            free_result_block(rb);
            free(line);
            // TODO: free other things too
            exit(EXIT_FAILURE);
        }
        // printf("The main term %u-%u has %u exception blocks\n",row/rb->r2+1, row%rb->r2+1,e); // Check

        // Initialize the exception blocks
        rb->terms[row] = create_empty_main_term(rb->c,e);

        // Get a pointer to the main term for easier working
        main_term *mt = &(rb->terms[row]);

        // Read the rest of the line
        // printf("Line: %s\n",line); // Check
        read_line(line, mt->row, true);
        // print_mat_line(mt->row,mt->c); // Check

        // Skip the unifier line
        getline(&line, &len, stream);

        // Read the exception blocks

        // TODO HERE: Change this to read the new exception format (with unifiers) and handle unifiers in the rest of the function too.
        // Read one by one the exception blocks
        if (e) read_exception_blocks(stream, mt, true);

        // Skip '% End: Exception subsetX' line
        // if (e) getline(&line, &len, stream);

        // Increment the row by 1
        row++;
    }

    free(line);
    free(mapping);
}

/**
 * Reads an operand block from a csv
 *
 * @param stream   Input file stream to operand block from, must point to the first line of the block, of form '% BEGIN: Matrix subsetX.Y (n,m)'
 * @return         Operand block that contains all main terms, their number of exception blocks and all the exceptions
 */
 operand_block read_operand_block(FILE *stream) {

    // Read operand block dimensions
    unsigned r, c;
    read_dimensions(stream, &r, &c);
    // printf("Operand block dimensions: %u rows, %u columns\n",r,c); // Check

    // Create the operand_block structure for later populating it
    operand_block ob = create_empty_operand_block(r, c);

    // Read the operand block and fill the struct
    read_operand_matrix(stream, &ob);

    return ob;
}

/**
 * Reads a result block from a csv
 * TODO: Update documentation
 *
 * @param stream   Input file stream to operand block from, must point to the first line of the block, of form '% BEGIN: Matrix subsetX.Y (n,m)'
 * @return         Operand block that contains all main terms, their number of exception blocks and all the exceptions
 */
result_block read_result_block(FILE *stream) {

    // Create the operand_block structure for later populating it
    result_block rb = create_null_result_block();

    // Read the operand block and fill the struct
    read_result_matrix(stream, &rb);

    return rb;
}

// ---------------------- READING FILE END ---------------------- //

// --------------------- CORE START --------------------- //
// Update the unifier of two elements from different rows, unifier pointer must be pointing to row_a's unifier (for now)
int unifier_a_b(int *row_a, int indexA, int *row_b, int indexB, int *unifier, int indexUnifier, int m1){

    // Get elements
    const int a = row_a[indexA];
    const int b = row_b[indexB];

    // A is constant
	if (a>0 && b > 0 && a!=b) // B is constant too and they don't match
		return -1;
	else if (a>0 && b <= 0)   // B is constant, add to unifier (b<-a)
    {
        unifier[1+indexUnifier]   = indexB + m1;
        unifier[1+indexUnifier+1] = indexA; 
    }
	else if (a <= 0) // A is variable and we don't care about B, add to unifier (a<-b)
	{
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB + m1; 
	}

	return 0;
}

// Return the unifier of two rows (naive) or -1 if not unificable
int unifier_rows(int *row_a, int *row_b, int *unifier, mgu_schema* ms3, int m1){
    int result;
    static unsigned m;
    m = ms3->n_common;
	for (unsigned i=0; i<m; i++)
	{
		result = unifier_a_b(row_a, ms3->common_L[i], row_b, ms3->common_R[i], unifier, 2*i, m1);
		if (result != 0) return result;
	}

	return 0;
}

// Correct/reduce/verify the unifier, unifier pointer must be pointing to row_a's unifier (for now)
int correct_unifier(int *row_a, int *row_b, int *unifier, int m, int m1){
    
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

        // If any of the two elements is a repeated variable, get real index
        if (x < m1 && row_a[x] < 0)
            x = -row_a[x] - 1;
        else if (x >= m1 && row_b[x-m1] < 0) 
            x = -row_b[x-m1] + m1 - 1;
        if (y < m1 && row_a[y] < 0) 
            y = -row_a[y] - 1;
        else if (y > m1 && row_b[y-m1] < 0)
            y = -row_b[y-m1] + m1 - 1; 

        // If any of them was substituted before, get the corresponding elements
        if (lst[x].count > 0) x = lst[x].by;
        if (lst[y].count > 0) y = lst[y].by;
        if (x==y) continue; 
        // No need to get real index again, the substitution is done for the real index

        // Get the value of x and y
        if (y >= m1) val_y = row_b[y-m1];
        else val_y = row_a[y];
        if (x >= m1) val_x = row_b[x-m1];
        else val_x = row_a[x];

        // printf("x: %d, val_x: %d\n",x,val_x); // Check
        // printf("y: %d, val_y: %d\n",y,val_y); // Check

        // If both constants 
        if (val_x > 0 && val_y > 0 && val_x!=val_y) return -1; // And don't match
        else if (val_x > 0 && val_y > 0) continue;             // And match

        // If x is constant and y is variable
        if (val_x > 0 && val_y == 0) // (y<-x)
        {   
            // make the replacement on y
            lst[y].count = 1;
            lst[y].by    = x;
            lst[y].ind   = y;

            // Update all variables replaced by y to be replaced by x
            L3 *current = lst[y].head;
            while (current != NULL)
            {
                lst[current->ind].by = x;
                current = current->next;
            }

            // Add the replacement list of y, and y itself, to replacement list of x
            if (lst[x].head)
            {
                lst[x].tail->next = create_L3(y,lst[y].head);
                if (lst[y].head) lst[x].tail = lst[y].tail;
            }
            else
            {
                lst[x].head = create_L3(y,lst[y].head);
                lst[x].tail = lst[x].head;
                if (lst[y].head) lst[x].tail = lst[y].tail;
            } 
            lst[y].head = lst[y].tail = NULL;
        }
        // If x is variable
        else // (x<-y)
        {
            // Make the replacement on x
            lst[x].count = 1;
            lst[x].by    = y;
            lst[x].ind   = x;

            // Update all variables replaced by y to be replaced by x
            L3 *current = lst[x].head;
            while (current != NULL)
            {
                int current_index = current->ind;
                // printf("Altering by of index %d\n",current_index); // Check
                lst[current_index].by = y;
                current = current->next;
            }

            // Add the replacement list of y, and y itself, to replacement list of x
            if (lst[y].head)
            {
                lst[y].tail->next = create_L3(x,lst[x].head);
                lst[y].tail = lst[y].tail->next;
                if (lst[x].head) lst[y].tail = lst[x].tail;

            }
            else
            {
                lst[y].head = create_L3(x,lst[x].head);
                lst[y].tail = lst[y].head;
                if (lst[x].head) lst[y].tail = lst[x].tail;
            }
            lst[x].head = lst[x].tail = NULL;
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
                unifier[1+last_unifier+1] = y;
                current = current->next;
                n_substitutions++;
                last_unifier+=2;
            }
        }
    }
    unifier[0] = n_substitutions;

    // Free lst
    for ( i = 0; i < 2*m; i++)
    {
        free_L2(lst[i]);
    }
    free(lst);
    
    return 0;
}

// Return the unifiers for two given matrices. Must have same width, and unifiers must be initialized to n0*n1 before calling function
int unifier_matrices(int *mat1, int *mat2, int n1, int n2, int *unifiers, matrix_schema* ms1, matrix_schema* ms2, mgu_schema* ms3){

    int i, j, code;
    int *unifier;

    unsigned last_unifier = 0;
    const unsigned m1 = ms1->m;
    const unsigned m2 = ms2->m;
    const unsigned m  = m1+m2-ms3->n_common;
    const unsigned unifier_size = 1+(2*m)+2;

    unifier = (int*) malloc (unifier_size*sizeof(int));

    for (i=0; i<n1; i++)
    {
        for (j=0; j<n2; j++)
        {
            memset(unifier,0,unifier_size*sizeof(int));  
            code = unifier_rows(&mat1[m1 * i], &mat2[m2 * j], unifier, ms3, m1);
            if (code != 0) continue; // Rows cannot be unified

            code = correct_unifier(&mat1[m1 * i], &mat2[m2 * j], unifier, m, m1);
            if (code != 0) continue; // Rows cannot be unified
            
            unifier[1+(2*m)]   = i;
            unifier[1+(2*m)+1] = j;
            memcpy(&unifiers[last_unifier*unifier_size],unifier,unifier_size*sizeof(int));
            last_unifier++;
        }
    }

    // Free the extra space
    unifiers = realloc(unifiers,last_unifier*unifier_size*sizeof(int));
    free(unifier);
    return last_unifier;
}

// Apply unifier to just row_a, change all occurrences of variable to constant
void apply_unifier_left(int *row_a, int *row_b, int *unifier, int m1){
    int n = unifier[0]*2;

    int i, j;
    int x, y, val_y; // To perform x <- y

    // Stuff needed if y is a variable
    int length = (int)log10(2*m1) + 2;
    char *y_str = (char *)malloc(length * sizeof(char));
    // clear(unif_dict); // Clean dictionary
    // printf("m1: %d, n: %d, length: %d\n",m1,n,length); // Check
    
    for (i = 1; i < n; i+=2)
    {
        x = unifier[i];
        y = unifier[i+1];
        
        if (x < m1)
        {
            if (y < m1) val_y = row_a[y];
            else val_y = row_b[y-m1];

            if (val_y > 0) // y is a constant, substitute first x reference for the constant in y
            {
                row_a[x] = val_y;
                for (j=0;j<m1;j++) if (row_a[j]==(-(x+1))) row_a[j] = val_y;
            }
            else // y is a variable, so it can get tricky
            {
                snprintf(y_str, length, "%d", y);
                struct nlist *entry = lookup(unif_dict,y_str);
                if (entry==NULL) // First appearance of y, do not substitute anything, but add appearance of y linked with x
                {
                    install(unif_dict,y_str,x);
                }
                else // Not first appearance: need to point all x references to previous (x<-y) [effectively (x<-(-z))]
                {
                    int z = entry->defn;
                    row_a[x] = -(z+1);
                    for (j=0;j<m1;j++) if (row_a[j]==(-(x+1))) row_a[j] = -(z+1);
                }
            }
        }
    }

    clear(unif_dict); // Clean dictionary
    free(y_str);
}

void prepare_unified(int *row_a, int *row_b, int *unified, matrix_schema* ms1, matrix_schema* ms2, mgu_schema* ms3)
{
    // printf("\nrow_a: "); print_mat_line(row_a, ms1->m); // Check
    //   printf("row_b: "); print_mat_line(row_b, ms2->m); // Check

    // Copy 'unified' row_a to start of new row
    memcpy(unified, row_a, ms1->m*sizeof(int));
    unsigned last_appended = ms1->m;

    // Append all elements not in common of row_b to new row
    for (unsigned i = 0; i < ms2->m; i++)
    {
        // printf("Checking if column %d of row_b is in mgu\n", ms2->columns[i]); // Check
        bool common = false;
        for (unsigned j = 0; j < ms3->n_common; j++)
        {
            // printf("Checking against columns %d of mgu\n", ms3->columns[j]); // Check
            if (ms2->columns[i]==ms3->common_columns[j])
            {
                // printf("    --->column %d of row_b found same with column %d of mgu !!\n", ms2->columns[i], ms3->columns[j]); // Check
                common=true;
                break;
            }
        }

        if (!common)
        {
            if (verbose) printf("appending column %d with index %d with value %d to position %d in unified\n",ms2->columns[i], ms2->mapping[i], row_b[ms2->mapping[i]], last_appended); // Check
            // printf("unified before: "); print_mat_line(unified, ms1->m+ms2->m-ms3->m); // Check
            memcpy(&unified[last_appended],&row_b[ms2->mapping[i]],sizeof(int));
            // printf("unified after:  "); print_mat_line(unified, ms1->m+ms2->m-ms3->m); // Check
            last_appended++;
        }   
    }

    // need an additional pass for reference-fixing

}
// --------------------- CORE END --------------------- //



int main(int argc, char *argv[]){
    struct timespec start_total, start_reading, start_unifiers, start_unification;
    struct timespec end_total, end_reading, end_unifiers, end_unification;     
    struct timespec elapsed, elapsed2;         

    const_dict = create_dictionary(1001);
    var_dict = create_dictionary(1001);
    unif_dict = create_dictionary(1001);

    char *M1_file = argv[1];
    char *M2_file = argv[2];
    char *M3_file = argv[3];
    if (argc>4) verbose = 1;

    // Open the files and check so
    FILE *stream_M1 = fopen(M1_file, "r");
    FILE *stream_M2 = fopen(M2_file, "r");
    FILE *stream_M3 = fopen(M3_file, "r");

    if (!stream_M1 || !stream_M2 || !stream_M3) {
        fprintf(stderr, "Error opening files %s, %s and %s; exiting\n", M1_file, M2_file, M3_file);
        if (stream_M1) fclose(stream_M1);
        if (stream_M2) fclose(stream_M2);
        if (stream_M3) fclose(stream_M3);
        exit(EXIT_FAILURE);
    }

    // Read number of blocks in M1 and M2
    unsigned s1, s2;
    if (read_num_blocks(stream_M1,&s1)) fprintf(stderr, "Could not read number of blocks in %s\n",M1_file);
    if (read_num_blocks(stream_M2,&s2)) fprintf(stderr, "Could not read number of blocks in %s\n",M2_file);

    printf("M1 blocks %u, M2 blocks %u\n",s1,s2); // Check

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_total);

    printf("M1 start ---------\n"); // Check
    operand_block ob;
    for (size_t s = 0; s < s1; s++)
    {
        ob = read_operand_block(stream_M1);
        // printf("%lu: - ",s+1); print_operand_block(&ob, 0); // Check
    }
    printf("Ignore, for avoiding optimization: %d\n",ob.r);
    printf("M1 end ---------\n"); // Check
    
    printf("M2 start ---------\n"); // Check
    for (size_t s = 0; s < s2; s++)
    {
        ob = read_operand_block(stream_M2);
        // printf("%lu: - ",s+1); print_operand_block(&ob, 0); // Check
    }
    printf("M2 end ---------\n"); // Check
    printf("Ignore, for avoiding optimization: %d\n",ob.r);

    

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_total);
    timespec_subtract(&elapsed, &end_total, &start_total);
    printf("Total time:                    %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);



    // Read the corresponding pair of blocks from M3

    result_block rb;
    rb = read_result_block(stream_M3);
    print_result_block(&rb,2);

    exit(EXIT_SUCCESS);
    // NEW: Read one by one blocks from M1 and M2
        // For each pair of blocks, read:
            // Their schemas
            // The number of exception blocks
            // The exceptions blocks
                // Their schemas and their schemas mappings


    int *mat0=NULL, *mat1=NULL, *mat2=NULL, n0,n1,n2,m0,m1,m2;
    matrix_schema *ms1, *ms2;
    mgu_schema *ms3;
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_total);

    // ----- read file start ----- //
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_reading);
    
    // read_mat_file(M1_file, &mat0,&mat1,&mat2,&n0,&n1,&m0,&m1,&n2,&m2,&ms1,&ms2);
    ms3 = create_mgu_from_mapping(NULL, m2, m0, m1); // THIS IS NOT CORRECT

    printf("Dimensions for M1 are (%d,%d) and for M2 are (%d,%d)\n",n0,m0,n1,m1);
    printf("MGU Dimensions: (%d,%d)\n",n2,m2);
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_reading);

    if (verbose)
    {
        print_matrix_schema(ms1);
        print_matrix_schema(ms2);
        if (ms3->n_common != 0) print_mgu_schema(ms3);

        printf("\nValues and metadata for M1 from %s\n",M1_file);
        print_mat_values(mat0,n0,m0);

        printf("\nValues and metadata for M2 from %s\n",M1_file);
        print_mat_values(mat1,n1,m1);

        printf("\nValues and metadata for MGU from %s\n",M1_file);
        print_mat_values(mat2,n2,m2);
    }
    // ----- read file end ----- //

    // ----- test all matrix start ----- //
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_unifiers);
    
    const unsigned m  = m0+m1-ms3->n_common;
	int *unifiers = NULL, unifier_size = 1+(2*m)+2;
    unifiers = (int*) malloc (n0*n1*unifier_size*sizeof(int));
    int unif_count = unifier_matrices(mat0, mat1, n0, n1, unifiers, ms1, ms2, ms3);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_unifiers);

    if (verbose) print_unifier_list(unifiers,unif_count,m);
    else printf("Number of unifiers: %d\n",unif_count);
    // ----- test all matrix end ----- //
    
    // ----- test unification start ----- //
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_unification);
    int *line_A  = (int*) malloc (m0*sizeof(int));
    int *line_B  = (int*) malloc (m1*sizeof(int));
    int *unified = (int*) malloc (unif_count*m*sizeof(int));

    int i, ind_A, ind_B;
    printf("Applying all unifiers . . . \n");
    for (i=0; i<unif_count; i++)
    {
        ind_A = unifiers[i*unifier_size+unifier_size-2];
        ind_B = unifiers[i*unifier_size+unifier_size-1];
        memcpy(line_A,&mat0[ind_A*m0],m0*sizeof(int));
        memcpy(line_B,&mat1[ind_B*m1],m1*sizeof(int));
        apply_unifier_left(line_A,line_B,&unifiers[i*unifier_size],m0);
        prepare_unified(line_A, line_B, &unified[i*m], ms1, ms2, ms3);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_unification);

    printf("Applied all unifiers\n");
    if (verbose) print_mat_values(unified,unif_count,m);
    // ----- test unification end ----- //

    // ----- test unification correct start ---- //
    printf("Comparing unification results. . . \n");
    // int same = compare_mgus(unified,mat2,unif_count,m);
    int same = unif_count == n2;
    if (same) printf("Unification is correct :)\n");
    else printf("Unification is NOT correct :(\n");
    // ----- test unification correct end   ---- //
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_total);

    printf("-------- TIME MEASUREMENTS --------\n");
    timespec_subtract(&elapsed, &end_reading, &start_reading);
    printf("Time for reading from file:    %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);

    timespec_subtract(&elapsed, &end_unifiers, &start_unifiers);
    printf("Time for calculating unifiers: %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);

    timespec_subtract(&elapsed2, &end_unification, &start_unification);
    printf("Time for applying unifiers:    %ld.%0*ld sec\n",elapsed2.tv_sec, 9, elapsed2.tv_nsec);
    
    timespec_add(&elapsed, &elapsed, &elapsed2);
    printf("Time for unification total:    %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);

    timespec_subtract(&elapsed, &end_total, &start_total);
    printf("Total time:                    %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);

    free(line_A);
    free(line_B);
    free(unified);
    free(mat0);
    free(mat1);
	return 0;
}
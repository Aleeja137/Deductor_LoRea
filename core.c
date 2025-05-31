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
Dictionary *var_dict;
Dictionary *unif_dict;
bool first_rb = true;

struct nlist *dict;
int verbose = 0;

// read_mat_row is necessary to prevent 'tok' from two matrices taking the same 'tok+row_str' 
// but row still needs to start from 0 each read_matrix call, so additional variable
int read_mat_row = 0; 
int last_int = 1;

struct timespec read_file_elapsed, unifiers_elapsed, unification_elapsed;

bool chivato = false;
bool global_correct = true;
unsigned global_count = 0;
unsigned global_incorrect = 0;

// --------------------- UTILS START --------------------- //

void print_L2_lst(L2 *lst, const unsigned n){

    // Chech max depth
    unsigned max_depth = 0;
    for (size_t i = 0; i < 2*n; i++)
    {
        unsigned depth = 0;
        L3 *tmp = lst[i].head;
        while (tmp!=NULL)
        {
            depth++;
            tmp=tmp->next;
        }
        if (depth>max_depth) max_depth = depth;
    }

    // Allocate memory to display L2 list
    int *ind     = (int*)malloc(2*n*sizeof(int));
    int *count   = (int*)malloc(2*n*sizeof(int));
    int *by      = (int*)malloc(2*n*sizeof(int));
    int *indices = (int*)malloc(max_depth*2*n*sizeof(int));
    memset(indices, -1, max_depth*2*n*sizeof(int));

    // Fill the spaces
    for (size_t i = 0; i < 2*n; i++)
    {
        ind[i] = lst[i].ind;
        count[i] = lst[i].count;
        by[i] = lst[i].by;

        unsigned idx = 0;
        L3 *tmp = lst[i].head;
        while (tmp!=NULL)
        {
            indices[max_depth*i+idx] = tmp->ind;
            tmp=tmp->next;
            idx++;
        }
    }

    // Display the L2 list
    printf("------------\n");
    printf("ind:\t");
    for (size_t i = 0; i < 2*n; i++)
    {
        printf("%d",ind[i]);
        if ((i+1)<(2*n)) printf(",");
    }
    printf("\n");
    printf("count:\t");
    for (size_t i = 0; i < 2*n; i++)
    {
        printf("%d",count[i]);
        if ((i+1)<(2*n)) printf(",");
    }
    printf("\n");
    printf("by:\t");
    for (size_t i = 0; i < 2*n; i++)
    {
        int val = by[i];
        if (val==-1) printf("X");
        else         printf("%d",val);
        if ((i+1)<(2*n)) printf(",");
    }
    printf("\n");
    
    for (size_t i = 0; i < max_depth; i++)
    {
        printf("\t");
        for (size_t j = 0; j < 2*n; j++)
        {
            int val = indices[max_depth*j+i];
            if (val==-1) printf("X");
            else         printf("%d",val);
            if ((j+1)<(2*n)) printf(",");
        }
        printf("\n");
    }
    
    // Free all memory:
    free(ind);
    free(count);
    free(by);
    free(indices);
}

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

// Print the mapping in a pretty way, taken from print_mgu_compact
void print_mapping(unsigned n_tl, unsigned *map) {
    for (unsigned i = 0; i < n_tl; i++) {
        unsigned left = map[2 * i];
        unsigned right = map[2 * i + 1];
        if (left == 0) printf("_");
        else printf("%u", left);
        printf("-");
        if (right == 0) printf("_");
        else printf("%u", right);
        if (i + 1 < n_tl) printf(",");
    }
    printf("\n");
}

// Prints the unifier, m is the number of columns in the resulting term
void print_unifier(unsigned *unifier, unsigned m){
    unsigned i;
    unsigned n_elem = unifier[0]*2;
    unsigned idxA = unifier[1+2*m];
    unsigned idxB = unifier[1+2*m+1];
    
    printf("%d elements: [",n_elem);
	for (i = 0; i < n_elem; i+=2)
	{
        printf("%u<-%u ", unifier[1+i],unifier[1+i+1]);
	}
    printf("],\t\tidxA: %u, idxB: %u\n",idxA+1, idxB+1);
}

void print_unifier_list(unsigned *unifiers, unsigned unif_count, unsigned m){

    unsigned i, unifier_size = 1+(2*m)+2;
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

int compare_main_terms(main_term *mt1, main_term *mt2){
    // Quick check
    if ((mt1->c != mt2->c) || (mt1->e != mt2->e))
    {
        if (verbose) {printf("compare_main_terms quick test failed\n");}
       return 0;
    }

    for (size_t i = 0; i < mt1->c; i++)
    {
        if (mt1->row[i]!=mt2->row[i])
        {
            if (verbose) {printf("compare_main_terms row test failed at element in column %lu\n",i);}
            if (verbose) {printf("my_rb:\t"); print_main_term(mt1,3,0);}
            if (verbose) {printf("rb   :\t"); print_main_term(mt2,3,0);}
            return 0;
        }
    }

    for (size_t i = 0; i < mt1->e; i++)
    {
        unsigned n = mt1->exceptions[i].n;
        unsigned m = mt1->exceptions[i].m;

        for (size_t j = 0; j < n; j++)
        {
            for (size_t k = 0; k < m; k++)
            {
                if (mt1->exceptions[i].mat[j*m+k] != mt2->exceptions[i].mat[j*m+k])
                {
                    printf("compare_main_terms exception test failed at exception block %lu, at exception %lu in column %lu\n",i,j,k);
                    printf("my_rb:\t"); print_exception_block(&mt1->exceptions[i],3,i+1);
                    printf("rb   :\t"); print_exception_block(&mt2->exceptions[i],3,i+1);
                    return 0;
                }
            }
            
        }
        
    }
    return 1;
}

// Return 0 if they are not the same
int compare_results(result_block *rb1, result_block *rb2, operand_block *ob1, operand_block *ob2){
    
    // Quick check
    if ((rb1->c1 != rb2->c1) || (rb1->c2 != rb2->c2) || (rb1->c != rb2->c) || 
    (rb1->r1 != rb2->r1) || (rb1->r2 != rb2->r2) || (rb1->r != rb2->r)) 
    // (rb1->t1 != rb2->t1) || (rb1->t2 != rb2->t2))
    {
        if (verbose) printf("compare_results quick test failed\n");
        if (verbose) printf("rb1: c1=%d, c2=%d, c=%d, r1=%d, r2=%d, r=%d, t1=%d, t2=%d\n",
            rb1->c1, rb1->c2, rb1->c, rb1->r1, rb1->r2, rb1->r, rb1->t1, rb1->t2);
        if (verbose) printf("rb2: c1=%d, c2=%d, c=%d, r1=%d, r2=%d, r=%d, t1=%d, t2=%d\n",
            rb2->c1, rb2->c2, rb2->c, rb2->r1, rb2->r2, rb2->r, rb2->t1, rb2->t2);
        return 0;
    }


    // Iterate the main terms, and check the rows, exceptions bloks, etc
    for (unsigned i = 0; i < rb1->r; i++)
    {
        if (rb1->valid[i] != rb2->valid[i]) {
            if (verbose) {printf("compare_results valid test failed at term %u (%u-%u)\n",i, i/rb1->r2+1, i%rb1->r2+1);}
            if (verbose) {printf("my valid: %u, csv valid: %u\n",rb1->valid[i], rb2->valid[i]);}
            if (verbose) {print_mgu_schema(rb2->ms);print_mgu_compact(rb2->ms);}
            return 0;}
        if (rb1->valid[i] == 0) // Only in this case, otherwise rows and exceptions will be empty
        {
            if (!compare_main_terms(&rb1->terms[i], &rb2->terms[i]))
            {
                if (verbose) {printf("compare_results main term test failed at term %u (%u-%u)\n",i, i/rb1->r2+1, i%rb1->r2+1);}
                if (verbose) {printf("mt1:\t"); print_main_term(&ob1->terms[i/rb1->r2],1,1);}
                if (verbose) {printf("mt2:\t"); print_main_term(&ob2->terms[i%rb1->r2],2,1);}
                if (verbose) {printf("mt3  (me):\t"); print_main_term(&rb1->terms[i],3,0);}
                if (verbose) {printf("mt3 (csv):\t"); print_main_term(&rb2->terms[i],3,0);}
                if (verbose) {print_mgu_schema(rb2->ms);print_mgu_compact(rb2->ms);}
                return 0;
            }

        }
    }
    return 1;
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
    char *tok = strtok(line, ",");
    if (skip_first) tok = strtok(NULL, ",\n");

    int col = 1;
    clear(var_dict);
    while (tok) {
        if (!isupper(tok[0])) { // If no uppercase appears, it is a constant
            const struct Symbol* s = get_value(tok, strlen(tok));
            row[col-1] = s->value;
        } else { // It is a variable
            if (lookup(var_dict, tok) == NULL) {
                row[col-1] = 0;
                install(var_dict, tok, col);
            } else {
                row[col-1] = -(lookup(var_dict, tok)->defn);
            }
        }
        tok = strtok(NULL, ",\n");
        col++;
    }
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
            mapping[count++] = (strcmp(first,  "_") == 0) ? 0 : (unsigned)atoi(first);
            mapping[count++] = (strcmp(second, "_") == 0) ? 0 : (unsigned)atoi(second);
        }
        tok = strtok(NULL, " ,\n");
    }
}

void read_exception_blocks(FILE *stream, main_term *mt, const bool result) {
    unsigned n, m;
    unsigned e = mt->e;
    exception_block *eb;
    char *line = NULL;
    size_t len = 0;

    for (unsigned i = 0; i < e; i++)
    {
        // Read dimensions of exception block (subset)
        read_dimensions(stream,&n,&m);

        // Create empty exception block to be populated later
        mt->exceptions[i] = create_empty_exception_block(n,m);
        eb = &(mt->exceptions[i]);

        // Skip unflatened schema
        getline(&line, &len, stream);

        // Read mapping
        getline(&line, &len, stream);
        unsigned *mapping = (unsigned*)malloc(eb->m*2*sizeof(unsigned));
        get_mapping(line,eb->m, mapping);
        eb->ms = create_mgu_from_mapping(mapping, eb->m, mt->c, eb->m);
        free(mapping);

        // Skip flatened schema if reading from operand
        if (!result) getline(&line, &len, stream);

        for (size_t j = 0; j < n; j++)
        {            
            getline(&line, &len, stream);
            char *line_ptr = line;  
            if (result) line_ptr = strchr(line_ptr, ':') + 2;
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
        char *line_copy = strdup(line);
        char *tok = strtok(line_copy, ",");
        unsigned e = (unsigned)strtoul(tok, NULL, 10);
        free(line_copy);
        
        // Initialize the exception blocks
        ob->terms[row] = create_empty_main_term(ob->c,e);

        // Get a pointer to the main term for easier working
        main_term *mt = &(ob->terms[row]);

        // Read the rest of the line
        read_line(line, mt->row, true);

        // Read one by one the exception blocks
        if (e) read_exception_blocks(stream, mt, false);

        // Increment the row by 1
        row++;
    }

    free(line);
}

void read_result_matrix(FILE *stream, result_block *rb) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read block header
    getline(&line, &len, stream);
    int matched = sscanf(line, "%% BEGIN: Matrix subset %u-%u (%u-%u,%u-%u,%u)",
                         &rb->t1, &rb->t2, &rb->r1, &rb->r2, &rb->c1, &rb->c2, &rb->c);
    
    if (matched != 7) {
        if (strstr(line, "END: Matrix M1 & M2 + MGU") != NULL) {free(line); return;}
        // if (verbose) 
        printf("line: '%s'\n",line);
        fprintf(stderr, "Could not read matrix subset info, matched: %d\n",matched);
        free(line);
        exit(EXIT_FAILURE);
    }

    // Info for all possible main terms from the unification of M1 block (r1) and M2 block (r2)
    rb->r = rb->r1*rb->r2;
    rb->terms = (main_term*)malloc(rb->r * sizeof(main_term));
    rb->valid = (unsigned*) malloc(rb->r * sizeof(unsigned));
    for (size_t aux = 0; aux < rb->r; aux++)
    {
        rb->valid[aux] = 0;
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
    unsigned row = 0;
    while ((read = getline(&line, &len, stream)) != -1 && row < rb->r) {
        
        // If end of matrix reached, exit
        if (strstr(line, "END") != NULL || strstr(line, "End") != NULL)
            break;
        
        // Get line indexes from row

        // Inspect if line is unifiable, subsumed or not unifiable
        if (strstr(line, "subsumed by exception") != NULL)
        {
            rb->valid[row] = 1;
            continue;
        }
        else if (strstr(line, "not unifiable") != NULL)
        {
            rb->terms[row] = create_null_main_term();
            rb->valid[row] = 2;
            row++;
            continue;
        }

        // If unifiable, read number of exception blocks
        unsigned d1, d2, e;
        if (sscanf(line, "Row %u-%u: %u", &d1, &d2, &e) != 3 &&
            sscanf(line, "Rows %u-%u: %u", &d1, &d2, &e) != 3) 
        {
            if (verbose) printf("Line is: %s\n",line);
            fprintf(stderr, "Could not read number of exception blocks in row\n");
            free(line);
            exit(EXIT_FAILURE);
        }

        // Initialize the exception blocks
        rb->terms[row] = create_empty_main_term(rb->c,e);
        if (rb->valid[row]!=1) rb->valid[row] = 0;

        // Get a pointer to the main term for easier working
        main_term *mt = &(rb->terms[row]);

        // Read the rest of the line
        read_line(line, mt->row, true);

        // Skip the unifier line
        getline(&line, &len, stream);

        // Read one by one the exception blocks
        if (e) read_exception_blocks(stream, mt, true);

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

    char *line = NULL;
    size_t len = 0;

    // Skip matrix header
    if (first_rb) {
        first_rb=false; 
        getline(&line, &len, stream);
        if (strstr(line, "END: Matrix M1 & M2 + MGU") != NULL) {free(line); return rb;}
    }

    // Read the operand block and fill the struct
    read_result_matrix(stream, &rb);

    free(line);
    return rb;
}

// ---------------------- READING FILE END ---------------------- //

// --------------------- CORE START --------------------- //
// Update the unifier of two elements from different rows, unifier pointer must be pointing to row_a's unifier (for now)
int unifier_a_b(int *row_a, const unsigned indexA, int *row_b, const unsigned indexB, unsigned *unifier, const unsigned indexUnifier, const unsigned cA, const unsigned n_cA, const unsigned cB){

    const unsigned m1 = cA + n_cA; // The number of columns in row_a is the old number of columns plus the new columns

    // A is from the new columns of row_a, that is, always a variable. Add (a<-b) to the unifier, don't care about B
    if (indexA >= cA)
    {
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB + m1; 
        return 0;
    } 
    // Else, B is from the new columns of row_b, so always a variable. Add (b<-a) to the unifier, don't care about A
    else if (indexB >= cB)
    {
        unifier[1+indexUnifier]   = indexB + m1;
        unifier[1+indexUnifier+1] = indexA; 
        return 0;
    }

    // If they are from the old columns, get elements
    const int a = row_a[indexA];
    const int b = row_b[indexB];

    // A is constant
	if (a>0 && b > 0 && a!=b) // B is constant too and they don't match
		return 1;
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

// Return the unifier of two rows (naive) or 1 if not unificable
int unifier_rows(main_term *mt1, main_term *mt2, mgu_schema *ms, unsigned *unifier){
    int result;
    const unsigned m = ms->n_common;
    for (unsigned i = 0; i < m; i++)
    {
        result = unifier_a_b(mt1->row, ms->common_L[i]-1, mt2->row, ms->common_R[i]-1, unifier, 2*i, mt1->c, ms->new_a, mt2->c);
        if (result) return result; // If it is not unifiable already, do not bother continuing
    }
    
	return 0;
}

// Correct/reduce/verify the unifier. The unifier has 1+2*m+2 elements, for n_substitutions + 2*m (max number of substitutions) and 2 for index of mt1 and mt2 in M1 and M2 respectively
int correct_unifier(main_term *mt1, main_term *mt2, mgu_schema *ms, unsigned *unifier){
    
    // const unsigned m1 = mt1->c + ms->new_a;
    // const unsigned m2 = mt2->c + ms->new_b;
    const unsigned c1 = mt1->c;
    const unsigned c2 = mt2->c;
    const unsigned m  = ms->n_common;
    const int *row_a = mt1->row;
    const int *row_b = mt2->row;
    unsigned lst_length = m*2;
    unsigned i;

    L2 *lst = (L2*) malloc (lst_length*sizeof(L2));
    for (i=0;i<lst_length;i++){
        lst[i] = create_L2_empty();
    }

    // For each element pair, get indexes and perform (x<-y)
    for (i=0; i<2*m; i+=2)
    {
        unsigned x = unifier[1+i]; 
        unsigned y = unifier[1+i+1];
        int val_y;
        int val_x; 

        if (x == y && x == 0) continue; // Empty case

        // If any of the two elements is a repeated variable, get real index
        if (x < c1 && row_a[x] < 0)
            x = -row_a[x] - 1;
        else if (x >= m && x < c2 && row_b[x-m] < 0) 
            x = -row_b[x-m] + m - 1;
        if (y < c1 && row_a[y] < 0) 
            y = -row_a[y] - 1;
        else if (y >= m && y < c2 && row_b[y-m] < 0)
            y = -row_b[y-m] + m - 1; 

        // If any of them was substituted before, get the corresponding elements
        if (lst[x].count > 0) x = lst[x].by;
        if (lst[y].count > 0) y = lst[y].by;
        if (x==y) continue; 
        // No need to get real index again, the substitution is done for the real index

        // Get the value of x and y
        if (x < c1) val_x = row_a[x];
        else if (x < m) val_x = 0;
        else if ((x-m) < c2) val_x = row_b[(x-m)];
        else val_x = 0;

        if (y < c1) val_y = row_a[y];
        else if (y < m) val_y = 0;
        else if ((y-m) < c2) val_y = row_b[(y-m)];
        else val_y = 0;

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
                if (!lst[x].tail) lst[x].tail = lst[x].head;
                lst[x].tail->next = create_L3(y,lst[y].head);
                if (lst[y].head) lst[x].tail = lst[y].tail;
            }
            else
            {
                lst[x].head = create_L3(y,lst[y].head);
                lst[x].tail = lst[x].head;
                if (lst[y].head) lst[x].tail = lst[y].tail;
            } 
            lst[y].head = NULL;
            lst[y].tail = NULL;
        }
        // If x is variable
        else // (x<-y)
        {
            // Make the replacement on x
            lst[x].count = 1;
            lst[x].by    = y;
            lst[x].ind   = x;

            // Update all variables replaced by x to be replaced by y
            L3 *current = lst[x].head;
            while (current != NULL)
            {
                int current_index = current->ind;
                lst[current_index].by = y;
                current = current->next;
            }

            // Add the replacement list of x, and x itself, to replacement list of y
            if (lst[y].head)
            {
                if (!lst[y].tail) lst[y].tail = lst[y].head;
                lst[y].tail->next = create_L3(x,lst[x].head);
                if (lst[x].head) lst[y].tail = lst[x].tail;

            }
            else
            {
                lst[y].head = create_L3(x,lst[x].head);
                lst[y].tail = lst[y].head;
                if (lst[x].head) lst[y].tail = lst[x].tail;
            }
            lst[x].head = NULL;
            lst[x].tail = NULL;
        }
    }

    int last_unifier = 0;
    unsigned n_substitutions = 0;
    // For each element, add the substitutions to the unifier
    for (i=0; i<lst_length; i++)
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
    for ( i = 0; i < lst_length; i++)
    {
        free_L2(lst[i]);
    }
    free(lst);
    
    return 0;
}

// Return the unifiers for two given matrices. Must have same width, and unifiers must be initialized to n0*n1 before calling function
unsigned unifier_matrices(operand_block *ob1, operand_block *ob2, result_block *rb, unsigned *unifiers){

    unsigned i, j, *unifier;
    int code;

    unsigned last_unifier = 0;
    // const unsigned m1 = ob1->c;
    // const unsigned m2 = ob2->c;
    const unsigned m  = rb->ms->n_common;
    const unsigned unifier_size = 1+(2*m)+2;

    unifier = (unsigned*) malloc (unifier_size*sizeof(unsigned));

    for (i=0; i<ob1->r; i++)
    {
        for (j=0; j<ob2->r; j++)
        {
            memset(unifier,0,unifier_size*sizeof(unsigned));  
            code = unifier_rows(&ob1->terms[i], &ob2->terms[j], rb->ms, unifier);
            if (code != 0) continue; // Rows cannot be unified

            code = correct_unifier(&ob1->terms[i], &ob2->terms[j], rb->ms, unifier);
            if (code != 0) continue; // Rows cannot be unified
            
            unifier[1+(2*m)]   = i;
            unifier[1+(2*m)+1] = j;
            memcpy(&unifiers[last_unifier*unifier_size],unifier,unifier_size*sizeof(unsigned));
            last_unifier++;
        }
    }

    // Free the extra space
    // unifiers = realloc(unifiers,last_unifier*unifier_size*sizeof(int));
    free(unifier);
    return last_unifier;
}

// Apply unifier to left term directly on the new main term
void apply_unifier_left(main_term *mt1, main_term *mt2, main_term *mt3, mgu_schema *ms, unsigned *unifier){
    
    const unsigned n = unifier[0]*2;
    const unsigned c1 = mt1->c;
    const unsigned c2 = mt2->c;
    const unsigned m  = ms->n_common; 
    unsigned i, j;
    unsigned x, y; 
    int val_y; // To perform (x<-y), x is ALWAYS a variable

    // Stuff needed if y is a variable
    int length = (int)log10(2*m) + 2;
    char *y_str = (char *)malloc(length * sizeof(char));
    
    // Copy the left row to the result row
    memcpy(mt3->row, mt1->row, c1*sizeof(int));
    int *row_a = mt3->row;
    const int *row_b = mt2->row;

    for (i = 1; i < n; i+=2)
    {
        x = unifier[i];
        y = unifier[i+1];
        
        if (x < m) // Only apply changes to left row
        {
            if (y < m) val_y = row_a[y];
            // else if (y < m) val_y = 0; // don't need this since row_a now is of size m, with the new columns already set to 0 on creation
            else if ((y-m) < c2) val_y = row_b[y-m];
            else val_y = 0;

            if (val_y > 0) // y is a constant, substitute all x references for the constant in y
            {
                row_a[x] = val_y;
                for (j=0;j<m;j++) if (row_a[j]==(int)(-(x+1))) row_a[j] = val_y;
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
                    for (j=0;j<m;j++) if (row_a[j]==(int)(-(x+1))) row_a[j] = -(z+1);
                }
            }
        }
    }

    clear(unif_dict); // Clean dictionary
    free(y_str);
}

// void prepare_unified(int *unified, int *row_b, mgu_schema* ms, bool reverse){
//     return;

//     // // Mapping is read main_term-main_term or main_term-exception, so get blind pointers in case we need to unify the exception (aka. in reverse order)
//     // unsigned n_uncommon_A     = reverse ? ms->n_uncommon_R     : ms->n_uncommon_L;
//     // unsigned n_uncommon_B     = reverse ? ms->n_uncommon_L     : ms->n_uncommon_R;
//     // // if (chivato) printf("--- prepare_unified ms printing:\n"); // Check
//     // // print_mgu_compact(ms,ms->n_common+ms->tot_n_uncommon_L+ms->tot_n_uncommon_R+ms->new); // Check
//     // // if (chivato) print_mgu_schema(ms); // Check
//     // // Only work if necessary
//     // if (n_uncommon_A == n_uncommon_B && n_uncommon_A == 0) return;

//     // unsigned tot_n_uncommon_A = reverse ? ms->tot_n_uncommon_R : ms->tot_n_uncommon_L;
//     // unsigned *idx_uncommon_B  = reverse ? ms->idx_uncommon_L   : ms->idx_uncommon_R;
//     // unsigned *uncommon_B      = reverse ? ms->uncommon_L       : ms->uncommon_R;
//     // unsigned tot_n_uncommon_B = reverse ? ms->tot_n_uncommon_L : ms->tot_n_uncommon_R;
//     // unsigned *common_B        = reverse ? ms->common_L         : ms->common_R;
//     // unsigned *common_A        = reverse ? ms->common_R         : ms->common_L;
//     // unsigned *addition_B      = reverse ? ms->addition_L       : ms->addition_R;
//     // unsigned n_common         = ms->n_common;

//     // // Pass 1: Append all elements not in common of row_b to new row
//     // unsigned i;
//     // unsigned last_appended = n_common + tot_n_uncommon_A;
//     // unsigned last_used = 0;

//     // for (i=0; i<n_uncommon_B; i++)
//     // {
//     //     unsigned start  = idx_uncommon_B[last_used]-1;
//     //     unsigned length = uncommon_B[2*i+1];
//     //     // if (chivato) {reverse ? printf("reverse ") : printf("not reverse ");} // Check
//     //     // if (chivato) printf("last_appended: %u, start: %u, length: %u, value at start: %d\n",last_appended,start,length,row_b[start]); // Check
//     //     memcpy(&unified[last_appended],&row_b[start],length*sizeof(int));
//     //     last_appended+=length;
//     //     last_used+=length;
//     // }

//     // // Pass 2: Fix reference from uncommon columns in R
//     // last_appended = n_common + tot_n_uncommon_A;
//     // for (i = 0; i < tot_n_uncommon_B; i++)
//     // {
//     //     // Get value in unified
//     //     int ref = unified[last_appended+i];
//     //     if (ref>=0) continue;

//     //     // If it is pointing somewhere, check if that column is common or not
//     //     unsigned reference = -(ref);
//     //     unsigned col_idx = 0;
//     //     bool found = false;
//     //     while (col_idx<n_common && !found)
//     //     {
//     //         if (common_B[col_idx]==reference) // Means that it IS a common column
//     //         {
//     //             found = true;
//     //             continue;
//     //         }
//     //         col_idx++;
//     //     }

//     //     if (found) {unified[last_appended+i] = -(common_A[col_idx]);}   
//     //     else 
//     //     {
//     //         col_idx = 0;
//     //         found = false;
//     //         while (col_idx<tot_n_uncommon_B && !found)
//     //         {
//     //             if (idx_uncommon_B[col_idx]==reference) // Means that it is NOT a common column
//     //             {
//     //                 found = true;
//     //                 continue;
//     //             }
//     //             col_idx++;
//     //         }
//     //         unsigned addition = addition_B[col_idx];
//     //         unified[last_appended+i] = -(idx_uncommon_B[col_idx] + addition + tot_n_uncommon_A);
//     //     }
//     // }
    
//     // // Pass 3: A general pass for propagating constants and references
//     // for (i = 0; i < n_common+tot_n_uncommon_A+tot_n_uncommon_B; i++)
//     // {
//     //     int ref = unified[i];
//     //     if (ref>=0) continue;
//     //     unsigned reference = -(ref+1);
//     //     if (unified[reference]!=0) {unified[i] = unified[reference];}
//     // }
// }

// // A subsums B if any of the two is true:
// // - No changes in A (strict) or ALL changes are in A, and are done by distinct variables of B (extended)
// bool subsums(const unsigned* unifier, const int *rowB, const unsigned m1, const unsigned m2)
// {
//     return false;

//     // bool *used = calloc(m2, sizeof(bool));
//     // bool change_in_B=false;
//     // if (!used) {
//     //     fprintf(stderr, "Error allocating used\n");
//     //     exit(EXIT_FAILURE);
//     // }

//     // for (unsigned i = 1; i < unifier[0]*2; i+=2) {
//     //     unsigned x = unifier[i];
//     //     unsigned y = unifier[i+1];

//     //     // If change done in A
//     //     if (x < m1) {

//     //         // All changes need to happen in A
//     //         if (change_in_B) {
//     //             free(used);
//     //             return false;
//     //         }

//     //         // If change comes from A too
//     //         if (y < m1) {
//     //             free(used); 
//     //             return false;
//     //         }
//     //         // If change comes from B
//     //         else 
//     //         {
//     //             // If constant, does not subsum
//     //             if (rowB[y-m1] > 0) {
//     //                 free(used); 
//     //                 return false;
//     //             }
//     //             // If first use of variable, set it as used
//     //             else if (!used[y-m1]) {used[y-m1] = true;}
//     //             // If not first use, does not subsum
//     //             else {
//     //                 free(used); 
//     //                 return false;
//     //             }
//     //         }
//     //     }
//     //     else change_in_B = true;
//     // }

//     // free(used);
//     // return true;
// }

// int check_exceptions(main_term *mt1, main_term *mt2, main_term *new_mt, main_term *read_mt){
//     return 0;

//     // // For each exception exception block from mt1
//     //     // For each exception within the exception block
//     //         // Inspect unifier with new_mt
//     //             // If they do not unify, skip
//     //             // If they unify, check if the subsums new_mt
//     //                 // If so, return 1 (subsumed by exception)
//     //                 // If not, apply unifier to exception and add to new_mt exception block (need read_mt mapping for this)

//     // unsigned i, j;
// 	// unsigned unifier_size, *unifier = NULL;

//     // unsigned n_exceptions, n_columns, new_n_columns;
//     // unsigned last_exc, n_common;
//     // unsigned total_exceptions = 0; // the number of exceptions added to the new_mt
    
//     // int *exception, *exc_mat, *new_exc_mat;
//     // int code;

//     // // if (chivato) printf("\tmt3.e: %u\n",read_mt->e); // Check
//     // // if (chivato) printf("\tmt1.e: %u\n",mt1->e); // Check
//     // // For each exception exception block from mt1
//     // for (i = 0; i < mt1->e; i++)
//     // {
//     //     n_exceptions  = mt1->exceptions[i].n;
//     //     n_columns     = mt1->exceptions[i].m;
//     //     new_n_columns = read_mt->exceptions[i].m;
        
//     //     exception    = (int*)malloc(n_columns*sizeof(int));                  // Will hold old exception
//     //     exc_mat      = mt1->exceptions[i].mat;                              // Pointer to the current matrix of exceptions
//     //     new_exc_mat  = (int*)malloc(n_exceptions*new_n_columns*sizeof(int)); // Matrix that will hold new unified exceptions
//     //     for (size_t _ = 0; _ < n_exceptions*new_n_columns; _++) {new_exc_mat[_] = 0;}
        
//     //     last_exc = 0;

//     //     n_common = read_mt->exceptions[i].ms->n_common; // This i changes to mt1->e+1 for exception blocks in M2
//     //     unifier_size = 1+(2*n_common)+2;
//     //     unifier = (unsigned*)malloc(unifier_size*sizeof(unsigned));

//     //     // if (chivato) printf("\t\tmt1->exceptions[%u] .n:%u, .m:%u, new_n_columns: %u, unifier_size: %u \n",i+1,n_exceptions,n_columns,new_n_columns,unifier_size); // Check
//     //     // For each exception within the exception block
//     //     for (j = 0; j < n_exceptions; j++)
//     //     {
//     //         // Inspect unifier with new_mt
//     //         memcpy(exception,&exc_mat[j*n_columns],n_columns*sizeof(int)); // Get exception to process (could be done after checking no subsumtion)
//     //         // if (chivato) {printf("\t\t\t og_exception: "); print_mat_line(exception,n_columns);} // Check
//     //         memset(unifier,0,unifier_size*sizeof(int)); // Reset unifier

// 	// 		code = unifier_rows(exception, new_mt->row, unifier, read_mt->exceptions[i].ms, n_columns, true);
// 	// 		if (code != 0) continue; // If they do not unify, skip
                
// 	// 		// code = correct_unifier(exception, new_mt->row, unifier, 2*n_common, n_columns, new_mt->c);
//     //         code = correct_unifier(&ob1->terms[i], &ob2->terms[j], rb->ms, unifier);
// 	// 		if (code != 0) continue; // If they do not unify, skip

//     //         // If they unify, check if the subsums new_mt
//     //         if (subsums(unifier,new_mt->row,n_columns,new_mt->c)) 
//     //         {
//     //             // if (chivato) printf("\t\t\t\t It subsums\n"); // Check
//     //             free(new_exc_mat);
//     //             free(exception);
//     //             free(unifier);
                
//     //             for (size_t _ = 0; _ < i; _++)
//     //             {
//     //                 free_exception_block(&new_mt->exceptions[_]);
//     //             }
//     //             free(new_mt->exceptions);
//     //             new_mt->exceptions = NULL;
//     //             return 1;
//     //         }

//     //         // If it does not subsum, apply unifier to exception and save exception for later creating the exception_block
//     //         apply_unifier_left(exception,new_mt->row,unifier,n_columns);
//     //         memcpy(&new_exc_mat[last_exc*new_n_columns], exception, n_columns*sizeof(int));
//     //         // if (chivato) {printf("\t\t\t mid_exception: "); print_mat_line(&new_exc_mat[last_exc*new_n_columns],n_columns);} // Check
//     //         // if (chivato) print_mgu_schema(read_mt->exceptions[i].ms); // Check
//     //         prepare_unified(&new_exc_mat[last_exc*new_n_columns], new_mt->row, read_mt->exceptions[i].ms, true);
//     //         // if (chivato) {printf("\t\t\t last_exception: "); print_mat_line(&new_exc_mat[last_exc*new_n_columns],n_columns);} // Check
//     //         last_exc++;
//     //         total_exceptions++;
//     //     }

//     //     // After all exceptions have been checked and unified, create the exception block
//     //     new_mt->exceptions[i] = create_exception_block(last_exc,new_n_columns,read_mt->exceptions[i].ms,new_exc_mat);

//     //     free(new_exc_mat);
//     //     free(exception);
//     //     free(unifier);
//     // }
    

//     // // Repeat everything for mt2
//     // // if (chivato) printf("\tmt2.e: %u\n",mt2->e); // Check
//     // for (i = 0; i < mt2->e; i++)
//     // {
//     //     n_exceptions  = mt2->exceptions[i].n;
//     //     n_columns     = mt2->exceptions[i].m;
//     //     new_n_columns = read_mt->exceptions[mt1->e+i].m;
        
//     //     exception    = (int*)malloc(n_columns*sizeof(int));                  // Will hold old exception
//     //     exc_mat      = mt2->exceptions[i].mat;                              // Pointer to the current matrix of exceptions
//     //     new_exc_mat  = (int*)malloc(n_exceptions*new_n_columns*sizeof(int)); // Matrix that will hold new unified exceptions
//     //     for (size_t _ = 0; _ < n_exceptions*new_n_columns; _++) {new_exc_mat[_] = 0;}
//     //     last_exc = 0;

//     //     n_common = read_mt->exceptions[mt1->e+i].ms->n_common; 
//     //     unifier_size = 1+(2*n_common)+2;
//     //     unifier = (unsigned*)malloc(unifier_size*sizeof(unsigned));

//     //     // For each exception within the exception block
//     //     // if (chivato) printf("\t\tmt1->exceptions[%u] .n:%u, .m:%u, new_n_columns: %u, unifier_size: %u \n",i+1,n_exceptions,n_columns,new_n_columns,unifier_size); // Check
//     //     for (j = 0; j < n_exceptions; j++)
//     //     {
//     //         // Inspect unifier with new_mt
//     //         memcpy(exception,&exc_mat[j*n_columns],n_columns*sizeof(int)); // Get exception to process (could be done after checking no subsumtion)
//     //         // if (chivato) {printf("\t\t\t og_exception: "); print_mat_line(exception,n_columns);} // Check
//     //         memset(unifier,0,unifier_size*sizeof(int)); // Reset unifier

// 	// 		code = unifier_rows(exception, new_mt->row, unifier, read_mt->exceptions[mt1->e+i].ms, n_columns, true);
// 	// 		if (code != 0) continue; // If they do not unify, skip
                
// 	// 		// code = correct_unifier(exception, new_mt->row, unifier, 2*n_common, n_columns, new_mt->c);
//     //         code = correct_unifier(&ob1->terms[i], &ob2->terms[j], rb->ms, unifier);
// 	// 		if (code != 0) continue; // If they do not unify, skip

//     //         // If they unify, check if the subsums new_mt
//     //         if (subsums(unifier,new_mt->row,n_columns,new_mt->c)) 
//     //         {
//     //             // if (chivato) printf("\t\t\t\t It subsums\n"); // Check
//     //             free(new_exc_mat);
//     //             free(exception);
//     //             free(unifier);

//     //             for (size_t _ = 0; _ < mt1->e+i; _++)
//     //             {
//     //                 free_exception_block(&new_mt->exceptions[_]);
//     //             }
//     //             free(new_mt->exceptions);
//     //             new_mt->exceptions = NULL;
//     //             return 1;
//     //         }

//     //         // If it does not subsum, apply unifier to exception and save exception for later creating the exception_block
//     //         apply_unifier_left(exception,new_mt->row,unifier,n_columns);
//     //         memcpy(&new_exc_mat[last_exc*new_n_columns], exception, n_columns*sizeof(int));
//     //         // if (chivato) {printf("\t\t\t mid_exception: "); print_mat_line(&new_exc_mat[last_exc*new_n_columns],n_columns);} // Check
//     //         // if (chivato) {print_mgu_schema(read_mt->exceptions[mt1->e+i].ms);} // Check
//     //         prepare_unified(&new_exc_mat[last_exc*new_n_columns], new_mt->row, read_mt->exceptions[mt1->e+i].ms, true);
//     //         // if (chivato) {printf("\t\t\t last_exception: "); print_mat_line(&new_exc_mat[last_exc*new_n_columns],n_columns);} // Check
//     //         last_exc++;
//     //         total_exceptions++;
//     //     }

//     //     // After all exceptions have been checked and unified, create the exception block
//     //     new_mt->exceptions[mt1->e+i] = create_exception_block(last_exc,new_n_columns,read_mt->exceptions[mt1->e+i].ms,new_exc_mat);

//     //     free(new_exc_mat);
//     //     free(exception);
//     //     free(unifier);
//     // }

//     // return 0;
// }

// void reorder_unified(main_term *mt, mgu_schema *ms){
//     return;

//     // unsigned i,j;
//     // int *after = (int*)malloc(mt->c*sizeof(int)); 
//     // int *before = mt->row;

//     // // This will tell me which column from after matches with which column of before
//     // unsigned *after_before = (unsigned*)malloc(mt->c*sizeof(unsigned)); 
//     // unsigned *before_after = (unsigned*)malloc(mt->c*sizeof(unsigned)); 

//     // unsigned idx_after, idx_before;

//     // for (i=0; i<ms->n_common; i++)
//     // {
//     //     idx_before = ms->common_L[i]-1;
//     //     idx_after  = ms->common_columns[i]-1;
//     //     after[idx_after] = before[idx_before];
//     //     after_before[idx_after]  = idx_before;
//     //     before_after[idx_before] = idx_after;
//     //     // printf("COMMON: idx_before: %u, idx_after: %u, value_before: %u, value after: %u\n",idx_before,idx_after,before[idx_before],after[idx_after]); // Check
//     // }
    
//     // unsigned last = 0;
//     // for (i=0; i<ms->n_uncommon_L; i++)
//     // {
//     //     unsigned start  = ms->uncommon_L[2*i]-1;
//     //     unsigned length = ms->uncommon_L[2*i+1];
//     //     for (j=0; j<length; j++)
//     //     {
//     //         idx_before = ms->idx_uncommon_L[last]-1;
//     //         idx_after  = start+j;
//     //         after[idx_after] = before[idx_before];
//     //         after_before[idx_after]  = idx_before;
//     //         before_after[idx_before] = idx_after;
//     //         last++;
//     //     }
//     // }
    
    
//     // last = 0;
//     // for (i=0; i<ms->n_uncommon_R; i++)
//     // {
//     //     unsigned start  = ms->uncommon_R[2*i]-1;
//     //     unsigned length = ms->uncommon_R[2*i+1];
//     //     for (j=0; j<length; j++)
//     //     {
//     //         idx_before = ms->idx_uncommon_R[last]-1;
//     //         idx_before += ms->tot_n_uncommon_L + ms->addition_R[last];
//     //         idx_after  = start+j;
//     //         after[idx_after] = before[idx_before];
//     //         after_before[idx_after]  = idx_before;
//     //         before_after[idx_before] = idx_after;
//     //         last++;
//     //     }
//     // }
    
//     // for (i=0; i<ms->new; i++)
//     // {
//     //     idx_before = mt->c-1-i; // not really true but more elegant and still correct
//     //     idx_after  = ms->new_indices[i]-1;
//     //     after[idx_after] = 0;
//     //     after_before[idx_after]  = idx_before;
//     //     before_after[idx_before] = idx_after;
//     // }
    
//     // // Final pass to correct reference indices
//     // for (idx_after = 0; idx_after < mt->c; idx_after++)
//     // {
//     //     int ref = after[idx_after];
//     //     if (ref>=0) continue; // Only continue if it points to somewhere in after

//     //     // Get the idx_before to see where it pointed to before
//     //     idx_before = after_before[idx_after];
//     //     ref = before[idx_before];
//     //     unsigned reference = -(ref+1);
//     //     // And the the reference in after to that index in after
//     //     after[idx_after] = -(before_after[reference]+1);
//     // }

//     // memcpy(before,after,mt->c*sizeof(int));

//     // free(after);
//     // free(before_after);
//     // free(after_before);
// }

// Given two operand blocks and a result block, performs the matrix intersection and returns the time 
void matrix_intersection(operand_block *ob1, operand_block *ob2, result_block *rb){

    struct timespec start_unifiers, start_unification;
    struct timespec end_unifiers, end_unification;     
    struct timespec elapsed, elapsed2;   
    
    // ----- Calculate unifiers start ----- //
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_unifiers);
    
	unsigned *unifiers = NULL;
    unsigned unifier_size = 1+(2*rb->ms->n_common)+2;
    unifiers = (unsigned*) malloc (ob1->r*ob2->r*unifier_size*sizeof(unsigned));
    unsigned unif_count = unifier_matrices(ob1, ob2, rb, unifiers);

    // print_unifier_list(unifiers,unif_count,rb->ms->n_common); // Check
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_unifiers);
    // ----- Calculate unifiers end ----- //
    
    // ----- Perform unification start ----- //
    if (verbose) printf("\tApplying all unifiers . . . \n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_unification);
    result_block my_rb = create_empty_result_block(ob1->r,ob2->r,ob1->c,ob2->c,rb->c,rb->ms);
    my_rb.t1 = 1;
    my_rb.t2 = 1;
    unsigned i, ind_A, ind_B;
    for (i=0; i<my_rb.r; i++) my_rb.valid[i] = 2;
    for (i=0; i<unif_count; i++)
    {
        // if (i==0) chivato=!chivato; // Check
        ind_A = unifiers[i*unifier_size+unifier_size-2];
        ind_B = unifiers[i*unifier_size+unifier_size-1];
        unsigned index_mt = ind_A*my_rb.r2+ind_B;

        main_term *mt = &my_rb.terms[index_mt];
        *mt = create_empty_main_term(my_rb.c, ob1->terms[ind_A].e + ob2->terms[ind_B].e);
        apply_unifier_left(&ob1->terms[ind_A], &ob2->terms[ind_B], mt, my_rb.ms, &unifiers[i*unifier_size]);
        
        // prepare_unified(mt->row,line_B, rb->ms, false);
        // reorder_unified(mt, rb->ms);
        // if (chivato) printf("Applied unifier to mt1-mt2: (%u-%u)\n",ind_A+1,ind_B+1); // Check
        // my_rb.valid[index_mt] = check_exceptions(&ob1->terms[ind_A], &ob2->terms[ind_B], mt, &rb->terms[index_mt]); // Not used for this version
        my_rb.valid[index_mt] = 0;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_unification);

    if (verbose) printf("\tApplied all unifiers\n");
    // ----- Perform unification end ----- //

    // ----- Check correctness start ---- //
    if (verbose) printf("\tComparing unification results. . . \n");
    int same_int = compare_results(&my_rb,rb,ob1,ob2);
    if (!same_int) {global_correct = false; global_incorrect++;}
    global_count++;

    if (same_int  && verbose) printf("Unification is correct :)\n");
    if (!same_int && verbose) printf("Unification is NOT correct :(\n");
    // ----- Check correctness end   ---- //

    // Accumulate the times
    timespec_subtract(&elapsed, &end_unifiers, &start_unifiers);
    timespec_add(&unifiers_elapsed, &unifiers_elapsed, &elapsed);

    timespec_subtract(&elapsed2, &end_unification, &start_unification);
    timespec_add(&unification_elapsed, &unification_elapsed, &elapsed2);

    // Free memory
    free(unifiers);
    free_result_block(&my_rb);
    
}
// --------------------- CORE END --------------------- //

int main(int argc, char *argv[]){
    struct timespec start_total, start_reading;
    struct timespec end_total, end_reading;     
    struct timespec elapsed;         

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_total);

    var_dict = create_dictionary(501);
    unif_dict = create_dictionary(501);

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
    printf("M1 blocks %u, M2 blocks %u, ",s1,s2); // Check

    operand_block *obs1 = (operand_block*)malloc(s1*sizeof(operand_block));
    operand_block *obs2 = (operand_block*)malloc(s2*sizeof(operand_block));
    result_block rb;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_reading);

    // ----- Read file start ----- //
    for (size_t i = 0; i < s1; i++)
    {
        obs1[i] = read_operand_block(stream_M1);
        // print_operand_block(&obs1[i],1,2); // Check
    }

    for (size_t i = 0; i < s2; i++)
    {
        obs2[i] = read_operand_block(stream_M2);
        // print_operand_block(&obs2[i],2,2); // Check
        // if (i==1) {print_main_term(&obs2[i].terms[140],2,1); exit(EXIT_FAILURE);}// Check
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_reading);
    timespec_subtract(&read_file_elapsed, &end_reading, &start_reading);    

    // ----- Read file end ----- //

    // ----- Matrix intersection start ----- //
    int check = 0; // Check - Select the matrix subset I want to work with
    // int check = 0; // Check - Work with all
    int ind = 0; // Check 
    do {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start_reading);
            rb = read_result_block(stream_M3);
            // print_result_block(&rb,2); // Check
        clock_gettime(CLOCK_MONOTONIC_RAW, &end_reading);
        if (rb.t1)
        {
            if (ind!=check) {free_result_block(&rb); ind++; continue;} // Check
            // verbose = true; // Check
            timespec_subtract(&elapsed, &end_reading, &start_reading);    
            timespec_add(&read_file_elapsed, &read_file_elapsed, &elapsed);
            if (verbose) print_result_block(&rb,0);
            matrix_intersection(&obs1[rb.t1-1],&obs2[rb.t2-1],&rb);
            free_result_block(&rb);
            // break; // Check
            ind++; // Check
            check++; // Check - use if starting from zero to get all blocks
        }
        else break;
    } while (true);
    // ----- Matrix intersection end ----- //

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_total);

    if (verbose) printf("-------- TIME MEASUREMENTS --------\n");
    if (verbose) printf("Time for reading from file:    %ld.%0*ld sec\n",read_file_elapsed.tv_sec, 9, read_file_elapsed.tv_nsec);
    if (verbose) printf("Time for calculating unifiers: %ld.%0*ld sec\n",unifiers_elapsed.tv_sec, 9, unifiers_elapsed.tv_nsec);
    if (verbose) printf("Time for applying unifiers:    %ld.%0*ld sec\n",unification_elapsed.tv_sec, 9, unification_elapsed.tv_nsec);

    if (global_correct) printf("OK, ");
    else printf("Not OK, ");
    printf("%u/%u, ", global_incorrect, global_count);
    printf("%ld.%0*ld, %ld.%0*ld, %ld.%0*ld, ",read_file_elapsed.tv_sec, 9, read_file_elapsed.tv_nsec,unifiers_elapsed.tv_sec, 9, unifiers_elapsed.tv_nsec,unification_elapsed.tv_sec, 9, unification_elapsed.tv_nsec);

    timespec_add(&unifiers_elapsed, &unifiers_elapsed, &unification_elapsed);
    if (verbose) printf("Time for unification total:    %ld.%0*ld sec\n",unifiers_elapsed.tv_sec, 9, unifiers_elapsed.tv_nsec);
    printf("%ld.%0*ld, ",unifiers_elapsed.tv_sec, 9, unifiers_elapsed.tv_nsec);
    timespec_subtract(&elapsed, &end_total, &start_total);
    if (verbose) printf("Total time:                    %ld.%0*ld sec\n",elapsed.tv_sec, 9, elapsed.tv_nsec);
    printf("%ld.%0*ld\n",elapsed.tv_sec, 9, elapsed.tv_nsec);
    // Free memory
    // Free operand blocks
    for (size_t i = 0; i < s1; i++) {
        free_operand_block(&obs1[i]);
    }
    for (size_t i = 0; i < s2; i++) {
        free_operand_block(&obs2[i]);
    }
    free(obs1);
    free(obs2);

    // Free dictionaries
    free_dictionary(var_dict);
    free_dictionary(unif_dict);

    fclose(stream_M1);
    fclose(stream_M2);
    fclose(stream_M3);
    
	return 0;
}
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

// Read matrix parameters
int read_parameters(char input_file[], int *n, int *m){

    FILE *stream;
	// Open the file and check access
	stream = fopen(input_file, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Error opening file %s, exiting\n", input_file);
		exit(EXIT_FAILURE);
	}

	char *line = NULL;
	size_t len = 0; 

    getline(&line, &len, stream);
    
    char *endptr, *e;
    long int num;
    int index;

    e = strchr(line, '(');
    index = (int)(e - line); // Improv: If values too big, could overflow int

    num = strtol(&line[index+1], &endptr, 10);
    if (endptr == line) fprintf(stderr, "Could not read matrix dimensions\n");
    *n = num;

    e = strchr(&line[index+1], ',');
    index += (int)(e - &line[index+1]) + 1;

    num = strtol(&line[index+1], &endptr, 10);
    if (endptr == line) fprintf(stderr, "Could not read matrix dimensions\n");
    *m = num;

    fclose(stream);
    return -1;
}

// Read matrix from file
void read_mat_file(char input_file[], int *mat, int n, int m)
{
	FILE *stream;
	char *line = NULL;
	char *tok;
	size_t len = 0; 
	ssize_t read;

    int start_elem = 1;
    // int start_unif = 1+m;
    int line_len = 1+m+1+(m*2)+2;
    int row = 0, col = 1; // Start indexing of columns from 1

	// Open the file and check access
	stream = fopen(input_file, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Error opening file %s, exiting\n", input_file);
		exit(EXIT_FAILURE);
	}

	// setrlimit // Improv full: Se podría hacer esto para limitar posibles errores en el fichero de lectura y que no explote la memoria (?)
	// Process the file line by line
	while ((read = getline(&line, &len, stream)) != -1 && row < n)
	{
		// Skip comment lines // Revise: Igual me interesa leer estas líneas para sacar info de la matrx (dimensiones?)
		if (line[0] == '%')
			continue;

		// Process line token by token
        mat[row*line_len] = m; // Add the number of elements in line
        mat[row*line_len+line_len-2] = row; // Add row index
		for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
		{
			// If a constant
			if (isdigit(tok[0]))
			{
                int index = (row*line_len) + col;
				mat[index] = atoi(tok);
            }
			else
			{
				// If variable not in dictionary, put a 0 and add index to dictionary
				if ((dict = lookup(tok)) == NULL)
				{
                    int index = (row*line_len) + col;
					mat[index] = 0;
                    install(tok, col);
				}
				// If variable IN dictionary, take the index as value
				else
				{
                    int index = (row*line_len) + col;
					mat[index] = -(dict->defn);
				}
			}
            col++;
		}
        // After processing the line, free dictionary
        clear(); // Improve: Quizá en lugar de liberar/reservar memoria línea a línea es mejor simplemente dejar que el diccionario crezca
        row++;
        col=1; // Start indexing of columns from 1
	}

	fclose(stream);
	if (line)
		free(line);
}

// Prints the matrix elements, not the metadata nor the unifiers
void print_mat_values(int *mat, int n, int m)
{
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
void print_mat_metadata(int *mat, int n, int m)
{
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
    printf("],\t\t\trowA: %d, rowB: %d\n",rowA, rowB);
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

    // printf("a: %d, indexA: %d, real_indexA: %d\n",a,indexA,real_indexA); // Check
    // printf("b: %d, indexB: %d, real_indexB: %d\n",b,indexB,real_indexB); // Check

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
        // printf("1. Unifier %d<-%d\n",indexA,indexB); // Check
	}

	return 0;
}

// Unifier must be pointing to row_a's unifier
int unifier_rows(int *row_a, int *row_b, int *unifier){
    int i, result;
    // printf("row_a[0]: %d, row_b[0]: %d\n",row_a[0],row_b[0]); // Check
    assert(row_a[0]==row_b[0]);
    int m = row_a[0];

	for (i=1; i<=m; i++)
	{
        unifier[0] = 0;
        // The unifier function handles the entire row including metadata, so pointers do not need to be modified
		result = unifier_a_b(row_a, i, row_b, i+m, unifier, 2*(i-1));
        // printf("indexed for unifier are %d and %d\n", 1+2*i, 1+2*i+1); // Check
        // printf("Unified row %d col %d, result is %d<-%d, CODE %d, n_elem unifier %d\n\n",row_a[1+m+1+2*m],i,unifier[1+2*i],unifier[1+2*i+1],result,unifier[0]); // Check
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
        int val_x; // Check

        if (x == y && x == 0) continue; // Empty case
        // printf("Initial (x<-y): (%d<-%d)\n",x,y); // Check
        // If an element is a repeated variable, get the real indexes
        if (x <= m && row_a[x] < 0) // x belongs to row_a and is a repeated variable
            x = -row_a[x];
        else if (x > m && row_b[x-m] < 0) // x belongs to row_b and is a repeated variable
            x = -row_b[x-m]+m;
        if (y <= m && row_a[y] < 0) // y belongs to row_a and is a repeated variable
            y = -row_a[y];
        else if (y > m && row_b[y-m] < 0)// x belongs to row_b and is a repeated variable
            y = -row_b[y-m]+m; 

        // Get the value of y
        if (y > m) val_y = row_b[y-m];
        else val_y = row_a[y];

        // Get the value of x just for checking  // Check
        if (x > m) val_x = row_b[x-m];  // Check
        else val_x = row_a[x];  // Check
        // printf("Real (x<-y): (%d<-%d), with values (%d<-%d)\n",x,y,val_x,val_y); // Check

        // Wanna do (x <- y), check the count of x
        if (lst[x-1].count == 0) // x not substituted, add the substitution on y
        {
            // printf("X was not substituted before, x: %d\n",x); // Check
            lst[x-1].count = 1;
            lst[x-1].by    = y;
            lst[x-1].ind   = x;

            if (lst[y-1].head)
            {
                L3* tmp = create_L3(x,lst[y-1].head);
                lst[y-1].head = tmp;
            }
            else
            {
                L3* tmp = create_L3(x,NULL);
                lst[y-1].head = lst[y-1].tail = tmp;
            }
        }
        else // If not zero, it means (x <- z) was done before. Check z
        {
            // printf("X was substituted before by z: %d, x: %d with count %d\n",lst[x].by,x,lst[x].count); // Check
            int z = lst[x-1].by; 
            int val_z;
            if (z > m) val_z=row_b[z-m];
            else val_z=row_a[z];

            if (z==y) continue; // It is the same substitution
            // printf("Real z: %d, with value: %d\n",z,val_z); // Check
            
            if (val_z > 0) // z is constant
            { 
                if (val_y > 0 && val_z != val_y) {return -1; printf("Check, not unifiable :(\n");} // y is constant
                else if (val_y == 0) // y is variable, add y<-z
                {
                    // printf("Z constant, Y variable, do (y<-z): (%d<-%d), with values (%d<-%d)\n",y,z,val_y,val_z); // Check
                    lst[y-1].count ++; 
                    lst[y-1].by  = z;
                    lst[y-1].ind = y;
                    
                    L3* tmp;
                    if (lst[y-1].head == NULL) 
                    {
                        tmp = create_L3(y,lst[z-1].head);
                        if (lst[z-1].head == NULL)
                        {
                            lst[z-1].head = lst[z-1].tail = tmp;
                        }
                        else
                        {
                            lst[z-1].head = tmp;
                        }
                    }
                    else
                    {
                        tmp = create_L3(y,lst[y-1].head);
                        if (lst[z-1].head == NULL)
                        {
                            lst[z-1].head = tmp;
                            lst[z-1].tail = lst[y-1].tail;
                        }
                        else
                        {
                            lst[z-1].head = tmp;
                            lst[y-1].tail = lst[z-1].head;
                        }
                    }
                }
            }
            else // z is variable, add z <- y
            {
                // printf("Z variable, Y don't care, do (z<-y): (%d<-%d), with values (%d<-%d)\n",z,y,val_z,val_y); // Check
                lst[z-1].count ++; 
                lst[z-1].by  = y;
                lst[z-1].ind = z;
                

                L3* tmp;
                if (lst[z-1].head == NULL) 
                {
                    tmp = create_L3(z,lst[y-1].head);
                    if (lst[y-1].head == NULL)
                    {
                        lst[y-1].head = lst[y-1].tail = tmp;
                    }
                    else
                    {
                        lst[y-1].head = tmp;
                    }
                }
                else
                {
                    tmp = create_L3(z,lst[z-1].head);
                    if (lst[y-1].head == NULL)
                    {
                        lst[y-1].head = tmp;
                        lst[y-1].tail = lst[z-1].tail;
                    }
                    else
                    {
                        lst[y-1].head = tmp;
                        lst[z-1].tail = lst[y-1].head;
                    }
                }
            }
        }
    }

    // printf("-----------\n"); // Check
    int last_unifier = 0;
    // For each element, add the substitutions to the unifier
    for (i=0; i<2*m; i++)
    {
        int y = i;
        int x;
        // printf("Info about y: %d, count %d, by %d, ind %d, head %p\n",y,lst[y].count,lst[y].by,lst[y].ind,lst[y].head); // Check
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
                // printf("(x<-y): (%d<-%d) \n",x,y); // Check
                n_substitutions++;
                last_unifier+=2;
            }
        }
    }
    unifier[0] = n_substitutions;
    // printf("-----------\n"); // Check
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
            
            // printf("Unifier rows %d and %d\n",i,j); // Check
            unifier[1+(2*m)]   = i;
            unifier[1+(2*m)+1] = j;
            memcpy(&unifiers[last_unifier*unifier_size],unifier,unifier_size*sizeof(int));
            // print_unifier(unifier,m); // Check
            // printf("\n"); // Check
            // print_mat_metadata(mat0,N,M); // Check
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
    /* A matrix is a set of lines
    Each line has the following info:
    m: num of elements in row, m elements, x: num of elements in unifier, x elements (x/2 substitutions), rowA: index in matA of rowA of unifier, rowB: index in matB of rowB of unifier
    In this way, each row has data and metadata about the row, and there is space allocated for unifier. 
    In case of unifying the row with multiple rows, the unifiers must be stored elsewhere.
    */

    // Check that char*-int dictionary works, also the new clean function
    // printf("\n ---Inserting elements to dictionary---\n");
    // install("a", 1);
    // install("c", 3);
    // install("c", 4);
    // if ((dict = lookup("a")) != NULL) printf("Found key %s with value %i\n",dict->name,dict->defn);
    // else printf("Key not found\n");

    // if ((dict = lookup("c")) != NULL) printf("Found key %s with value %i\n",dict->name,dict->defn);
    // else printf("Key not found\n");

    // clear(); printf(" ------Clearing dictionary------\n");
    // if ((dict = lookup("c")) != NULL) printf("Found key %s with value %i\n",dict->name,dict->defn);
    // else printf("Key not found c\n");

    // printf(" ---Inserting elements to dictionary---\n");
    // install("a", 1);
    // if ((dict = lookup("a")) != NULL) printf("Found key %s with value %i\n",dict->name,dict->defn);
    // else printf("Key not found\n\n");

    // For development, I will work with set and known sizes N and M
    char *csv_file_1 = "correcto1.csv";
    char *csv_file_2 = "correcto2.csv";
    int n0, n1, m0, m1;
    read_parameters(csv_file_1,&n0,&m0);
    read_parameters(csv_file_2,&n1,&m1);
    assert(m0==m1);
    int row_size = 1+m0+1+(2*m0)+2;
    printf("size of int %ld\n",sizeof(int)*8);
    printf("Number of elements per row: %d\n",row_size);
    printf("Number of elements for mat0: %d\n",n0*(row_size));
    printf("Number of elements for mat1: %d\n",n1*(row_size));

    int *mat0 = (int*) malloc(n0*row_size*sizeof(int));
    int *mat1 = (int*) malloc(n1*row_size*sizeof(int));

	read_mat_file(csv_file_1, mat0, n0, m0);

	printf("read_mat_file completed :)\n");
    printf("Dimensions for mat0 are (%d,%d)\n",n0,m0);

	printf("\nValues and metadata from %s\n",csv_file_1);
    print_mat_values(mat0,n0,m0);
    // print_mat_metadata(mat0,n0,m0);

	read_mat_file(csv_file_2, mat1, n1, m1);
	printf("read_mat_file completed :)\n");
    printf("Dimensions for mat1 are (%d,%d)\n",n1,m1);
    printf("\nValues and metadata from %s\n",csv_file_2);
    print_mat_values(mat1,n1,m1);
    // print_mat_metadata(mat1,n1,m1);
    // printf("----------------\n");

    // Check that the unifier for two elements works -------------
    // int row = 0, col = 2, i;
    // int *unifier = &mat0[1+M];
    // unifier[0] = 2*col;
    // for (i=0; i<2*col; i++) unifier[1+i] = i;

    // printf("Testing unifier of row %d col %d from files %s y %s:\n",row, col, csv_file_1,csv_file_2);
    // int code = unifier_a_b(&mat0[row],col,&mat0[row],col+M,unifier,col*2);
    // printf("Code: %d\n",code);
    // print_mat_metadata(mat0,n0,m0);

    // Check that unifier for two rows works ----------------
    // int row = 0;
    // int *unifier = &mat0[1+M]; // Store the unifier in rowA (left row to be unified)
    // int code = unifier_rows(&mat0[row_size * row], &mat1[row_size * row], unifier);
    // if (code == 0) 
    // {
    //     print_mat_metadata(mat0,n0,m0);
    //     code = correct_unifier(&mat0[row_size * row], &mat1[row_size * row], unifier);

    // }
    // else 
    // {
    //     printf("Could not unify rows before processing\n");
    // }
    // if (code == 0) 
    // {
    //     print_mat_metadata(mat0,n0,m0);
    // }
    // else 
    // {
    //     printf("Could not unify rows\n");
    // }


	// char *unifier = unify_rows(mat0,mat1,M);
	// printf("Unifier: %s\n",unifier);
	
	// unify_matrices(mat0,mat1,N,M);
	int i,*unifiers = NULL, unif_count, unifier_size = 1+(2*m0)+2;
    unifiers = (int*) malloc (n0*n1*unifier_size*sizeof(int));
    unif_count = unifier_matrices(mat0, mat1, n0, n1, unifiers);
    printf("unif_count: %d\n",unif_count);
    for (i=0; i<unif_count; i++)
    {
        print_unifier(&unifiers[i*unifier_size],m0);
    }

    free(mat0);
    free(mat1);
	printf("\nMain Completed, argument count %d, program name %s\n", argc, argv[0]);
	return 0;
}
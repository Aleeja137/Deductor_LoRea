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
#define N 4
#define M 6

struct nlist *dict;

// Read matrix from file
void read_mat_file(char input_file[], int *mat, int m)
{
	FILE *stream;
	char *line = NULL;
	char *tok;
	size_t len = 0; 
	ssize_t read;

    int start_elem = 1;
    // int start_unif = 1+m;
    int line_len = 1+m+1+(m*2)+2;
    int row = 0, col = 0;

	// Open the file and check access
	stream = fopen(input_file, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Error opening file %s, exiting\n", input_file);
		exit(EXIT_FAILURE);
	}

	// setrlimit // Improv full: Se podría hacer esto para limitar posibles errores en el fichero de lectura y que no explote la memoria (?)
	// Process the file line by line
	while ((read = getline(&line, &len, stream)) != -1)
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
                int index = (row*line_len) + start_elem + col;
				mat[index] = atoi(tok);
            }
			else
			{
				// If variable not in dictionary, put a 0 and add index to dictionary
				if ((dict = lookup(tok)) == NULL)
				{
                    int index = (row*line_len)+start_elem+col;
					mat[index] = 0;
                    install(tok, col);
				}
				// If variable IN dictionary, take the index as value
				else
				{
                    int index = (row*line_len)+start_elem+col;
					mat[index] = -(dict->defn);
				}
			}
            col++;
		}
        // After processing the line, free dictionary
        clear(); // Improve: Quizá en lugar de liberar/reservar memoria línea a línea es mejor simplemente dejar que el diccionario crezca
        row++;
        col=0;
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

// Unify two elements from different rows
int unifier_a_b(int *row_a, int indexA, int *row_b, int indexB, int *unifier, int indexUnifier){

    // Make sure both rows have same number of elements
    int m = row_a[0];
    assert(row_a[0]==row_b[0]); 

    // Get elements (+m is added to second row, might be reversed from recursive calls)
    int a, b, real_indexA = indexA+1, real_indexB = indexB+1;
    if (real_indexA >= m) real_indexA = real_indexA-m; 
    if (real_indexB >= m) real_indexB = real_indexB-m;
    a = row_a[real_indexA];
    b = row_b[real_indexB];

    printf("a: %d, indexA: %d, real_indexA: %d\n",a,indexA,real_indexA); // Check
    printf("b: %d, indexB: %d, real_indexB: %d\n",b,indexB,real_indexB); // Check

    // A is constant (or variable initialized to constant)
	if (a>0 && b > 0 && a!=b) // B is constant too
		return -1;
	else if (a>0 && b <= 0)        // B is variable
		return unifier_a_b(row_b,indexB,row_a,indexA,unifier,indexUnifier);

	// A is unitialized variable
	if (a == 0 && b > 0) // B is constant
	{
        // Update unifier (a<-b)
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB; 
        unifier[0]+=2;
        printf("1. Unifier %d<-%d\n",indexA,indexB); // Check
	}
	else if (a == 0 && b <= 0) // B is variable
	{
        // Update unifier (a<-b)
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB;
        unifier[0]+=2;
        printf("2. Unifier %d<-%d\n",indexA,indexB); // Check
	}

    // A is variable initialized to other variable
    if (a < 0) // B is constant, (a <- b), a being the index of the variable it points to
    {   
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB;
        unifier[0]+=2;
        printf("3. Unifier %d<-%d\n",-a,indexB); // Check
    }

	return 0;
}

// Unifier must be pointing to row_a's unifier
int unifier_rows(int *row_a, int *row_b, int *unifier){
    int i, result;
    assert(row_a[0]==row_b[0]);
    int m = row_a[0];

	for (i=0; i<m; i++)
	{
        // The unifier function handles the entire row including metadata, so pointers do not need to be modified
		result = unifier_a_b(row_a, i, row_b, i+m, unifier, 2*i);
        // printf("indexed for unifier are %d and %d\n", 1+2*i, 1+2*i+1); // Check
        printf("Unified row %d col %d, result is %d<-%d, n_elem unifier %d\n\n",row_a[1+m+1+2*m],i,unifier[1+2*i],unifier[1+2*i+1],unifier[0]); // Check
		if (result != 0) return result;
	}

	return 0;
}

// Unifier must be pointing to row_a's unifier
int correct_unifier(int *row_a, int *row_b, int *unifier){
    int m = row_a[0];
    assert(row_a[0]==row_b[0]);
    
    int i, n_substitutions = 0;
    L2 *lst = (L2*) malloc (2*m*sizeof(L2));
    for (i=0;i<2*m;i++){
        lst[i] = create_L2_empty();
    }

    // For each element pair, get indexes for (x<-y)
    for (i=0; i<2*m; i+=2)
    {
        int x = unifier[1+i];
        int y = unifier[1+i+1];
        int val_y;

        // If an element is a repeated variable, get the real indexes
        if (x < m && row_a[1+x] < 0) // x belongs to row_a and is a repeated variable
            x = -row_a[1+x];
        else if (x > m && row_b[1+x-m] < 0) // x belongs to row_b and is a repeated variable
            x = -row_b[1+x]+m;
        
        if (y < m && row_a[1+y] < 0) // y belongs to row_a and is a repeated variable
            y = -row_a[1+y];
        else if (x > m && row_b[1+y-m] < 0) // x belongs to row_b and is a repeated variable
            y = -row_b[1+y]+m; 

        // Get the values of y
        if (y > m) val_y = row_b[1+y-m];
        else val_y = row_a[1+y];
        
        // Wanna do (x <- y), check the count of x
        if (lst[x].count == 0) // If zero, add the substitution on y 
        {
            lst[x].count = 1;
            lst[x].by    = y;
            lst[x].ind   = x;

            if (lst[y].head)
            {
                L3* tmp = create_L3(x,lst[y].head);
                lst[y].head = tmp;
            }
            else
            {
                L3* tmp = create_L3(x,NULL);
                lst[y].head = lst[y].tail = tmp;
            }
        }
        else // If not zero, it means (x <- z) was done before. Check z
        {
            int z = lst[x].by; 
            int val_z;
            if (z > m) val_z=row_b[1+z-m];
            else val_z=row_a[1+z];

            
            if (val_z > 0) // z is constant
            { 
                if (val_y > 0 && val_z != val_y) {return -1;} // y is constant
                else if (val_y == 0) // y is variable, add y<-z
                {
                    lst[y].count ++; // Revise: Puede y tener un count != 0 ?
                    lst[y].by  = z;
                    lst[y].ind = y;
                    
                    if (lst[z].head)
                    {
                        L3* tmp = create_L3(y,lst[z].head);
                        lst[z].head = tmp;
                    }
                    else
                    {
                        L3* tmp = create_L3(y,NULL);
                        lst[z].head = lst[z].tail = tmp;
                    }
                }
            }
            else // z is variable, add z <- y
            {
                lst[z].count ++; // Revise: Puede z tener un count != 0 ?
                lst[z].by  = y;
                lst[z].ind = z;
                
                if (lst[y].head)
                {
                    L3* tmp = create_L3(z,lst[y].head);
                    lst[y].head = tmp;
                }
                else
                {
                    L3* tmp = create_L3(z,NULL);
                    lst[y].head = lst[y].tail = tmp;
                }
            }
        }
    
        n_substitutions++;
    }

    // For each element, add the substitutions to the unifier
    for (i=0; i<2*m; i+=2)
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
                unifier[1+i]   = x;
                unifier[1+i+1] = y;
                current = current->next;
                n_substitutions++;
            }
        }
    }
    unifier[0] = n_substitutions;

    return 0;
}

// char** unify_matrices(L1 **m0, L1 **m1, int nrow, int ncol)
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
    n0=n1=N;
    m0=m1=M;
    int row_size = 1+m0+1+(2*m0)+2;

    printf("Number of elements per row: %d\n",row_size);
    printf("Number of elements for all: %d\n",n0*(row_size));
    int *mat0 = (int*) malloc(n0*row_size*sizeof(int));
    int *mat1 = (int*) malloc(n1*row_size*sizeof(int));

	read_mat_file(csv_file_1, mat0, m0);
	read_mat_file(csv_file_2, mat1, m1);
	printf("read_mat_file completed :)\n");

	printf("\nValues and metadata from %s\n",csv_file_1);
    print_mat_values(mat0,n0,m0);
    print_mat_metadata(mat0,n0,m0);

    printf("\nValues and metadata from %s\n",csv_file_2);
    print_mat_values(mat1,n1,m1);
    print_mat_metadata(mat1,n1,m1);
    printf("----------------\n");

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
    int row = 0;
    int *unifier = &mat0[1+M]; // Store the unifier in rowA (left row to be unified)
    int code = unifier_rows(&mat0[row_size * row], &mat1[row_size * row], unifier);
    if (code == 0) code = correct_unifier(&mat0[row_size * row], &mat1[row_size * row], unifier);
    printf("Code: %d\n",code);
    print_mat_metadata(mat0,n0,m0);

	// // update_pointers(mat0[2], mat1[2]);
	// printf("---\nmat0[2] L2 addr: %p\n",mat0[2]->info);
	// printf("mat1[2] L2 addr: %p\n---\n",mat0[2]->info);
	// printf("mat0[2] L2 name: %s\n",mat0[2]->info->me->name);
	// printf("mat1[2] L2 name: %s\n",mat0[2]->info->me->name);

	// char *unifier = unify_rows(mat0,mat1,M);
	// printf("Unifier: %s\n",unifier);
	
	// unify_matrices(mat0,mat1,N,M);
	

	// free_L1_mat(N,M,mat); // Improve: Cómo detecto si una posición de memoria se ha liberado?
	printf("\nMain Completed, argument count %d, program name %s\n", argc, argv[0]);
	return 0;
}
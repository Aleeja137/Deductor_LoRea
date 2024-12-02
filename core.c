#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#include "dictionary.h"

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
	if (a>0 && b <= 0)        // B is variable
		return unifier_a_b(row_b,indexB,row_a,indexA,unifier,indexUnifier);

	// A is unitialized variable
	if (a == 0 && b > 0) // B is constant
	{
        // Unify
		// row_a[real_indexA] = b;

        // Update unifier (a<-b)
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB; 
	}

	if (a == 0 && b <= 0) // B is variable
	{
        // Unify
        // row_a[real_indexA] = b;

        // Update unifier (a<-b)
        unifier[1+indexUnifier]   = indexA;
        unifier[1+indexUnifier+1] = indexB;
        unifier[0]+=2;
	}

	return 0;
}

// char* unify_rows(L1 **m0, L1 **m1, int ncol)
// char** unify_matrices(L1 **m0, L1 **m1, int nrow, int ncol)
// void read_unif_mat_file();
// void process_files();

int main(int argc, char *argv[])
{
    /* Una matriz es un conjunto de líneas
    Cada línea tiene la siguiente info:
    m: número de elementos, m elementos, x: número de elementos en el unificador, x parejas de elementos (unificador), rowA: índice de la matriz A, rowB, índice de la matriz B
    De esta manera, cada fila tiene información sobre la fila, la fila, información sobre el unificador y el unificador
    */

    // Comprobar que el diccionario de string-int funciona
    printf("\n ---Insertando elementos al diccionario---\n");
    install("a", 1);
    install("c", 3);
    install("c", 4);
    if ((dict = lookup("a")) != NULL) printf("Encontrada key %s con valor %i\n",dict->name,dict->defn);
    else printf("No se encontró la key\n");

    if ((dict = lookup("c")) != NULL) printf("Encontrada key %s con valor %i\n",dict->name,dict->defn);
    else printf("No se encontró la key\n");

    clear(); printf(" ------Limpiando diccionario------\n");
    if ((dict = lookup("c")) != NULL) printf("Encontrada key %s con valor %i\n",dict->name,dict->defn);
    else printf("No se encontró la key c\n");

    printf(" ---Insertando elementos al diccionario---\n");
    install("a", 1);
    if ((dict = lookup("a")) != NULL) printf("Encontrada key %s con valor %i\n",dict->name,dict->defn);
    else printf("No se encontró la key\n\n");

    // Para desarrollo, trabajaré con tamaños N y M fijos y previamente conocidos  
    char *csv_file_1 = "correcto1.csv";
    char *csv_file_2 = "correcto2.csv";
    int n0, n1, m0, m1;
    n0=n1=N;
    m0=m1=M;

    printf("Number of elements per row: %d\n",1+m0+1+2*m0+2);
    printf("Number of elements for all: %d\n",n0*(1+m0+1+2*m0+2));
    int *mat0 = (int*) calloc(n0*(1+m0+1+2*m0+2),sizeof(int));
    int *mat1 = (int*) calloc(n1*(1+m1+1+2*m1+2),sizeof(int));

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

	// char *unifier = "";
	// unifier = unify_a_b(mat0[2],mat1[2],unifier);
	// printf("Unifier: %s\n",unifier);
    int row = 0, col = 2, i;
    int *unifier = &mat0[1+M];
    unifier[0] = 2*col;
    for (i=0; i<2*col; i++) unifier[1+i] = i;

    printf("Testing unifier of row %d col %d from files %s y %s:\n",row, col, csv_file_1,csv_file_2);
    int code = unifier_a_b(&mat0[row],col,&mat0[row],col+M,unifier,col*2);
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
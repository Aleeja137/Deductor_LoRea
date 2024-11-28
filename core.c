#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>

#include "structures.h"
#include "dictionary.h"

// #define N 50
// #define M 10
#define N 4
#define M 6

struct nlist *dict;

// Declaración funciones
void update_pointers(L1 *a, L1 *b);

// Leer matriz desde fichero
void read_mat_file(char input_file[], L1 **mat)
{
	FILE *stream;
	char *line = NULL;
	char *tok;
	size_t len = 0, last_mat_element = 0;
	ssize_t read;

	// Abrir fichero y verificar acceso
	stream = fopen(input_file, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Error opening file %s, exiting\n", input_file);
		exit(EXIT_FAILURE);
	}
	// else printf("File %s opened correctly :) \n", input_file); // Check

	// setrlimit // Improv full: Se podría hacer esto para limitar posibles errores en el fichero de lectura y que no explote la memoria (?)
	// Procesar fichero línea a línea
	while ((read = getline(&line, &len, stream)) != -1)
	{
		// Saltarme líneas de comentario // Revise full: Igual me interesa leer estas líneas para sacar info de la matrx (dimensiones?)
		if (line[0] == '%')
			continue;

		// Procesar línea token a token
		for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
		{
			// Si es constante
			if (isdigit(tok[0]))
			{
				// Crear nodo L1 con ese valor int
				mat[last_mat_element] = create_L1(atoi(tok), NULL, tok);
				last_mat_element++;
				// printf("const mat[%lu]: %d for char %s\n",last_mat_element-1, mat[last_mat_element-1]->val,tok); // Check
				// printf("Const: %s\n",tok); // Check
				// Improv full: viendo que las constantes se repiten mucho, podría usar el diccionario para que todas las mismas constantes apunten al mismo L1 (más espacio de diccionario, mayor tiempo procesamiento de lectura de matriz, pero menos memoria de programa). Para ver cómo, ver el siguiente else
			}
			else
			{
				// printf("Var: %s\n",tok); // Check
				// If the variable does not exist in the dictionary, create a L1 with 0 for value. Add entry to dictionary with the string with the memory address (https://stackoverflow.com/questions/73711419/how-to-convert-variables-address-to-string-variable-in-c)
				if ((dict = lookup(tok)) == NULL)
				{
					L1 *temp_L1 = create_L1(0, NULL, tok);
					L2 *temp_L2 = create_L2(0, NULL, NULL, temp_L1);
					temp_L1->info = temp_L2;
					mat[last_mat_element] = temp_L1;
					last_mat_element++;
					// 17 porque son 2 caracteres por byte + el char de fin de línea
					char addr[17]; // Improv: el tamaño se puede reducir?
					sprintf(addr, "%p", temp_L1);
					// printf("Addr: %s\n", addr); // Check
					install(tok, addr);
					// printf("Character %s stored in %s\n",tok,addr); // Check
				}
				// If the variable exists in the dictionary, point to the L1, taking the address from the dictionary (https://stackoverflow.com/questions/15081863/string-to-address-c)
				else
				{
					char *addr = dict->defn;
					L1 *temp_L1;
					sscanf(addr, "%p", &temp_L1);
					mat[last_mat_element] = temp_L1;
					last_mat_element++;
					// printf("Character %s taken from %s\n",tok,addr); // Check
				}
				// printf("var mat[%lu]: %d for char %s\n",last_mat_element-1, mat[last_mat_element-1]->val,tok); // Check
			}
		}
	}

	fclose(stream);
	if (line)
		free(line);
}

// Imprime matriz con chars, solo conviene usarla con matrices pequeñas
void print_mat(L1 **mat, int n, int m)
{

	int i, j;
	for (i = 0; i < n; i++)
	{
		printf("[");
		for (j = 0; j < m; j++)
			printf("%s ", mat[i * m + j]->name);
		printf("]\n");
	}
}

// Unifica dos elementos
char* unify_a_b(L1 *a, L1 *b, char *unifier){

	// printf("a.val %d, a.name %s\n",a->val,a->name); // Check
	// printf("b.val %d, b.name %s\n",b->val,b->name); // Check

	// A es constante (o variable inicializada a constante)
	if (a->val > 0 && b->val > 0 && a->val!=b->val)
	// {printf("Check opt 1\n"); // Check
		return NULL;
	// } // Check
	if (a->val > 0 && b->val <= 0) 
	// {printf("Check opt 2\n");// Check
		return unify_a_b(b,a,unifier);
	// }// Check

	// A es variable sin inicializar
	if (a->val == 0 && b->val > 0)
	{
		// printf("Check opt 3\n"); // Check
		a->val = b->val;
		if (a->info == NULL) 
		// {printf("Created L2\n");  // Check
			a->info = create_L2(b->val, NULL, NULL, a);
		// } // Check
		else 
		// {printf("Assigned L2 value\n"); // Check
			a->info->val = b->val;
		// } // Check
		char *this_unification=strdup(a->name);
		// printf("First concat, this_unification = %s\n",this_unification); // Check
		strcat(this_unification,"<-");
		// printf("Second concat, this_unification = %s\n",this_unification); // Check
		strcat(this_unification,b->name);
		// printf("Third concat, this_unification = %s\n",this_unification); // Check
		if (strlen(unifier) != 0)
		{
			char * tmp_unifier = strdup(unifier);
			strcat(tmp_unifier,",");
			// printf("Len %zu\n",strlen(unifier)); // Check
			// printf("Fourth concat, unifier = %s\n",tmp_unifier); // Check
			strcat(tmp_unifier,this_unification);
			// printf("Done concat, unifier = %s\n",tmp_unifier); // Check
			return tmp_unifier; // omega.append(a<-b)
		}
		else return this_unification;
		
	}

	if (a->val == 0 && b->val <= 0)
	{
		// printf("Check opt 4\n"); // Check
		update_pointers(a,b);
		char *this_unification=strdup(a->name);
		strcat(this_unification,"<-");
		strcat(this_unification,b->name);
		if (strlen(unifier) != 0)
		{
			char * tmp_unifier = strdup(unifier);
			strcat(tmp_unifier,",");
			// printf("Len %zu\n",strlen(unifier)); // Check
			// printf("Fourth concat, unifier = %s\n",tmp_unifier); // Check
			strcat(tmp_unifier,this_unification);
			// printf("Done concat, unifier = %s\n",tmp_unifier); // Check
			return tmp_unifier; // omega.append(a<-b)
		}
		else return this_unification;
	}

	// printf("Check opt 5\n"); // Check
	return unifier;
}

// Actualiza variables para hacer la sustitución a<-b
void update_pointers(L1 *a, L1 *b){
	// Si la info de variable está vacía, crearla
	if (b->info == NULL)
	// { // Check
		// printf("%s.info era NULL\n",b->name); // Check
		b->info = create_L2(0,NULL,NULL,b);
	// } // Check
	// Añadir x al final de la lista de variable info
	if (b->info->tail == NULL)
	{
		// printf("%s.info.tail era NULL, añadiendo un L3 con %s\n",b->name, a->name); // Check
		b->info->head = create_L3(a,NULL);
		b->info->tail = b->info->head;			
	}
	else
	{
		// printf("%s.info.tail No era NULL, añadiendo un L3 con %s al final\n",b->name, a->name); // Check
		b->info->tail->next = create_L3(a,NULL);
		b->info->tail = b->info->tail->next;
	}

	// Guardar entry y añadir variable info de x al 
	// final de variable info de y
	L3 *entry = b->info->tail;
	if (a->info->head != NULL)
	{
		// printf("%s.info tenía elementos, añadirlos al final de la lista de %s.info\n",a->name, b->name); // Check
		// printf("'Dueño' del %s.info: %s\n",a->name, a->info->me->name); // Check
		b->info->tail->next = a->info->head;
		b->info->tail = a->info->tail;
	}

	// Recorrer todas las variables y que su variable info 
	// apunte a variable info de y
	L3 *current = entry;
	while (current != NULL)
	{
		// printf("current: %s, cambiando a %s.info\n",current->var->name, b->name); // Check
		// Liberar el variable info de x (todas las variables
		// sustituídas por x usan ese variable info)
		if (current->next == NULL)
		// { // Check
		// 	printf("liberado el %s.info\n",a->name); // Check
			free_L2(current->var->info);
		// } // Check
		current->var->info = b->info;
		current = current->next;
	}

	// printf("---\n %s L2 addr: %p\n",a->name,a->info); // Check
	// printf(" %s L2 addr: %p\n---\n",b->name, b->info); // Check
}

char* unify_rows(L1 **m0, L1 **m1, int ncol){
	int i;
	char *unifier = "";

	for (i=0; i<ncol; i++)
	{
		// printf("Unifying %s and %s\n",m0[i]->name, m1[i]->name); // Check
		unifier = unify_a_b(m0[i],m1[i],unifier);
		if (unifier == NULL) return NULL;
	}

	return unifier;
}

char** unify_matrices(L1 **m0, L1 **m1, int nrow, int ncol){

	int i, j, last_unifier = 0;
	char **unifier_lst = NULL;

	for (i=0; i<nrow; i++)
	for (j=0; j<nrow; j++)
	{
		// printf("Check 1!!\n"); // Check
		L1 **tmp_m0 = copy_mat(1,ncol,&m0[i*ncol]);
		// printf("Check 2!!\n"); // Check
		L1 **tmp_m1 = copy_mat(1,ncol,&m1[j*ncol]);
		printf("Unifying rows %d and %d\n",i,j); // Check
		char *unifier=strdup("");
		unifier = unify_rows(tmp_m0,tmp_m1,ncol);
		printf("Unfier for rows %d and %d is %s\n",i,j,unifier); // Check
		if (unifier != NULL) 
		{
			unifier_lst = (char **) realloc(unifier_lst,(last_unifier+1)*sizeof(char *));
			unifier_lst[last_unifier] = strdup(unifier);
			last_unifier++;
		}
	}
	return unifier_lst;
}

// Leer matriz unificada desde cero y sus unificadores
void read_unif_mat_file();
// Procesar ficheros, sólo uno, un directorio, etc
void process_files();

int main(int argc, char *argv[])
{
	L1 **mat0 = constr_mat_vacia(N, M);
	L1 **mat1 = constr_mat_vacia(N, M);
	read_mat_file("benchmark_test/test00.csv", mat0);
	read_mat_file("benchmark_test/test01.csv", mat1);
	// printf("read_mat_file completed :)\n");
	// printf("Mat 0 from benchmark_test/test00.csv\n");
	// print_mat(mat0, N, M);
	// printf("Mat 1 from benchmark_test/test01.csv\n");
	// print_mat(mat1, N, M);

	// char *unifier = "";
	// unifier = unify_a_b(mat0[2],mat1[2],unifier);
	// printf("Unifier: %s\n",unifier);

	// // update_pointers(mat0[2], mat1[2]);
	// printf("---\nmat0[2] L2 addr: %p\n",mat0[2]->info);
	// printf("mat1[2] L2 addr: %p\n---\n",mat0[2]->info);
	// printf("mat0[2] L2 name: %s\n",mat0[2]->info->me->name);
	// printf("mat1[2] L2 name: %s\n",mat0[2]->info->me->name);

	// char *unifier = unify_rows(mat0,mat1,M);
	// printf("Unifier: %s\n",unifier);
	
	unify_matrices(mat0,mat1,N,M);
	

	// free_L1_mat(N,M,mat); // Improve: Cómo detecto si una posición de memoria se ha liberado?
	printf("Main Completed, argument count %d, program name %s\n", argc, argv[0]);
	return 0;
}
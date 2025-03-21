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

#define ROW_STR_SIZE (snprintf(NULL, 0, "%d", INT_MAX) + 1)
#define chivato printf("HEY <..........................................................................\n");

struct nlist *dict;
int verbose = 0;

// read_mat_row is necessary to prevent 'tok' from two matrices taking the same 'tok+row_str'
// but row still needs to start from 0 each read_matrix call, so additional variable
int read_mat_row = 0;
int last_int = 1;

// --------------------- UTILS START --------------------- //
void timespec_add(struct timespec *result, const struct timespec *t1, const struct timespec *t2)
{
	result->tv_sec = t1->tv_sec + t2->tv_sec;
	result->tv_nsec = t1->tv_nsec + t2->tv_nsec;
	if (result->tv_nsec >= 1000000000L)
	{
		result->tv_sec++;
		result->tv_nsec -= 1000000000L;
	}
}

// Prints just a single matrix line
void print_mat_line(int *row, int m)
{
	int j;
	printf("[");
	for (j = 0; j < m; j++)
		printf("%d ", row[j]);
	printf("]\n");
}

// Prints the matrix elements, not the metadata, nor exceptions nor the unifiers
void print_matrix_clean(int *mat, int *LUT, int n, int m)
{
	int i;
	int line_index;

	for (i = 0; i < n; i++)
	{
		line_index = LUT[i * 2];
		print_mat_line(&mat[line_index * m], m);
	}
}

void print_matrix_clean_no_LUT(int *mat, int n, int m)
{
	int i;
	for (i = 0; i < n; i++)
	{
		print_mat_line(&mat[i * m], m);
	}
}

void print_matrix_full(int *mat, int *LUT, int n, int m)
{
	int i, j;
	int row_idx, n_exceptions;

	for (i = 0; i < n; i++)
	{
		row_idx = LUT[i * 2];
		n_exceptions = LUT[i * 2 + 1];

		print_mat_line(&mat[row_idx * m], m);
		for (j = 0; j < n_exceptions; j++)
		{
			printf("\t");
			print_mat_line(&mat[(row_idx + 1 + j) * m], m);
		}
	}
}
// Prints the unifier
void print_unifier(int *unifier, int m)
{
	int i;
	int n_elem = unifier[0] * 2;
	int rowA = unifier[1 + 2 * m];
	int rowB = unifier[1 + 2 * m + 1];

	printf("%d elements: [", n_elem);
	for (i = 0; i < n_elem; i += 2)
	{
		printf("%d<-%d ", unifier[1 + i], unifier[1 + i + 1]);
	}
	printf("],\t\t\trowA: %d, rowB: %d\n", rowA + 1, rowB + 1);
}

// Prints a list of unifiers
void print_unifier_list(int *unifiers, int unif_count, int m)
{

	int i, unifier_size = 1 + (2 * m) + 2;
	for (i = 0; i < unif_count; i++)
	{
		print_unifier(&unifiers[i * unifier_size], m);
	}
	printf("Number of unifiers: %d\n", unif_count);
}

// Prints the LUT content [starts_idx, n_exceptions]
void print_LUT(int *LUT, int n)
{
	int i;
	for (i = 0; i < n; i++)
		printf("\t[%d,%d]\n", LUT[i * 2], LUT[i * 2 + 1]);
}

// Compares if two MGU matrices are equal
int compare_mgus(int *my_mgu, int *other_mgu, int n, int m)
{
	int same = 1;
	int n_elems = n * m;
	int i;

	for (i = 0; i < n_elems; i++)
	{
		if (my_mgu[i] != other_mgu[i])
		{
			int row = i / m;
			int col = i % m;
			printf("\n");
			printf("My result: ");
			print_mat_line(&my_mgu[row * (1 + m)], m);
			printf("Original:  ");
			print_mat_line(&other_mgu[row * (1 + m)], m);
			printf("Different elements at row %d col %d: %d != %d\n", row, col+1, my_mgu[i], other_mgu[i]);
			return 0;
		}
	}
	return same;
}
// ---------------------- UTILS END ---------------------- //

// --------------------- READING FILE START --------------------- //
void read_matrix_dimensions(FILE *stream, int *n, int *m, int *n_exceptions)
{
	char *line = NULL;
	size_t len = 0;
	char *endptr, *e;
	long int num;

	getline(&line, &len, stream);
	if (strstr(line, "BEGIN") == NULL)
	{
		fprintf(stderr, "Could not read dimensions, function called on wrong line\n");
		fprintf(stderr, "Line: %s\n", line);
		exit(EXIT_FAILURE);
	}

	// Move until first digit
	e = strchr(line, '(');
	if (!e)
	{
		fprintf(stderr, "Could not read dimensions\n");
		exit(EXIT_FAILURE);
	}

	// Read dimension n
	num = strtol(e + 1, &endptr, 10);
	if (endptr == e + 1)
	{
		fprintf(stderr, "Could not read dimensions\n");
		exit(EXIT_FAILURE);
	}
	*n = num;

	// Move until second digit
	e = strchr(endptr, ',');
	if (!e)
	{
		fprintf(stderr, "Could not read dimensions\n");
		exit(EXIT_FAILURE);
	}

	// Read dimension m
	num = strtol(e + 1, &endptr, 10);
	if (endptr == e + 1)
	{
		fprintf(stderr, "Could not read matrix dimensions\n");
		exit(EXIT_FAILURE);
	}
	*m = num;

	// Move until third digit
	e = strchr(endptr, ',');
	if (!e)
	{
		fprintf(stderr, "Could not read dimensions\n");
		exit(EXIT_FAILURE);
	}

	// Read dimension n_exceptions
	num = strtol(e + 1, &endptr, 10);
	if (endptr == e + 1)
	{
		fprintf(stderr, "Could not read matrix dimensions\n");
		exit(EXIT_FAILURE);
	}
	*n_exceptions = num;

	// ---- v1 processing starts ----
	getline(&line, &len, stream);
	getline(&line, &len, stream);
	// ---- v1 processing ends ----
	free(line);
}

void read_matrix(FILE *stream, int **matrix, int **LUT, int n, int m, int tot_exc)
{
	char *line = NULL;
	char row_str[ROW_STR_SIZE];
	size_t len = 0;
	ssize_t read;
	int row = 0, col, line_len = m;
	*matrix = (int *)malloc((n + tot_exc) * line_len * sizeof(int));
	*LUT = (int *)malloc(n * 2 * sizeof(int));
	int first_tok = 1;
	int n_line = 1;
	int start_line = 0;
	int exception_lines = 0;
	int n_exc = 0;

	while ((read = getline(&line, &len, stream)) != -1 && n_line <= n)
	{
		if (strstr(line, "END") != NULL)
			break;

		// ---- MGU processing starts ----
		if (strstr(line, "Unifier") != NULL)
			continue;
		if (strstr(line, "M1 -->") != NULL || strstr(line, "M2 -->") != NULL)
			continue;
		if (strstr(line, "Row") != NULL)
		{
			char *start = strchr(line, ':');
			if (start)
			{
				memmove(line, start + 2, strlen(start));
			}
		}
		// ---- MGU processing ends ----

		// ---- v1 processing starts ----
		if ((strchr(line, ':') != NULL) && (strchr(line, '[') != NULL))
			continue;
		// ---- v1 processing ends ----

		col = 1; // Start indexing columns from 1
		char *tok = strtok(line, ",");
		while (tok)
		{
			// If it is a main term, read the number of exceptions, reserve the extra memory needed, and continue reading
			if (first_tok)
			{
				// Read the number of exceptions
				n_exc = atoi(tok);
				first_tok = 0;
				// printf("Number of exceptions: %d\n", n_exc); // Check

				// Update lookup table
				(*LUT)[((n_line - 1) * 2)] = start_line;
				(*LUT)[((n_line - 1) * 2) + 1] = n_exc;
				start_line += 1 + n_exc;
				// n_line += 1;
				exception_lines = 0;

				// Read next token
				tok = strtok(NULL, ",\n");
			}
			else
			{
				// If not first token, read elements normally and go storing them
				int index = (row * line_len) + col - 1;
				if (isupper(tok[0]))
				{ // If first character is uppercase, it is a variable
					snprintf(row_str, sizeof(row_str), "%d", read_mat_row);
					char temp_tok[ROW_STR_SIZE + strlen(tok)];
					snprintf(temp_tok, sizeof(temp_tok), "%s%s", tok, row_str);

					if (lookup(temp_tok) == NULL)
					{
						(*matrix)[index] = 0;

						install(temp_tok, col);
						// if (verbose) printf("VARIABLE read element '%s' first appearance, assigned column %d\n", tok, col); // Check
					}
					else
					{
						(*matrix)[index] = -(lookup(temp_tok)->defn);
						// if (verbose) printf("VARIABLE read element '%s' NOT first appearance, assigned index %d\n", tok, (*matrix)[index]); // Check
					}
				}
				else
				{ // It is a constant
					if (lookup(tok) == NULL)
					{
						(*matrix)[index] = last_int;
						install(tok, last_int);
						last_int++;
						// if (verbose)	printf("CONSTANT read element '%s' first appearance, assigned integer %d\n", tok, (*matrix)[index]); // Check
					}
					else
					{
						(*matrix)[index] = (lookup(tok)->defn);
						// if (verbose) printf("CONSTANT read element '%s' NOT first appearance, assigned integer %d\n", tok, (*matrix)[index]); // Check
					}
				}
				tok = strtok(NULL, ",\n");
				col++;
			}
		}

		// Between rows, check if last exception is read. If so, set first_tok to 1
		if (exception_lines == n_exc)
		{
			first_tok = 1;
			n_line++;
		}
		else
			exception_lines++;

		row++;
		read_mat_row++;
	}
	free(line);
}

void read_mat_file(char input_file[],
				   int **mat1, int **LUT1, int *n1, int *m1, int *exc1,
				   int **mat2, int **LUT2, int *n2, int *m2, int *exc2,
				   int **mat3, int **LUT3, int *n3, int *m3, int *exc3)
{
	FILE *stream = fopen(input_file, "r");
	if (!stream)
	{
		fprintf(stderr, "Error opening file %s, exiting\n", input_file);
		exit(EXIT_FAILURE);
	}
	clear();

	// Read first matrix
	read_matrix_dimensions(stream, n1, m1, exc1);
	// printf("Dimensions for M1 are (%d,%d,%d)\n", *n1, *m1, *exc1); // Check
	read_matrix(stream, mat1, LUT1, *n1, *m1, *exc1);

	// Read second matrix
	read_matrix_dimensions(stream, n2, m2, exc2);
	// printf("Dimensions for M2 are (%d,%d,%d)\n", *n2, *m2, *exc2); // Check
	read_matrix(stream, mat2, LUT2, *n2, *m2, *exc2);

	// Read MGU matrix
	read_matrix_dimensions(stream, n3, m3, exc3);
	// printf("MGU Dimensions: (%d,%d,%d)\n", *n3, *m3, *exc3); // Check
	read_matrix(stream, mat3, LUT3, *n3, *m3, *exc3);
	clear();

	fclose(stream);
}
// ---------------------- READING FILE END ---------------------- //

// --------------------- CORE START --------------------- //
// Update the unifier of two elements from different rows, unifier pointer must be pointing to row_a's unifier (for now)
int unifier_a_b(int *row_a, int indexA, int *row_b, int indexB, int *unifier, int m, int indexUnifier)
{
	// Get elements (+m is added to second row, might be reversed from recursive calls, so need to check)
	int a, b, real_indexA = indexA, real_indexB = indexB;
	if (indexA > m)
		real_indexA = real_indexA - m;
	if (indexB > m)
		real_indexB = real_indexB - m;
	a = row_a[real_indexA];
	b = row_b[real_indexB];

	// A is constant (or variable initialized to constant)
	if (a > 0 && b > 0 && a != b) // B is constant too
		return -1;
	else if (a > 0 && b <= 0) // B is variable
		return unifier_a_b(row_b, indexB, row_a, indexA, unifier, m, indexUnifier);

	// A is variable
	if (a <= 0) // B is constant
	{
		// Update unifier (a<-b)
		unifier[1 + indexUnifier] = indexA;
		unifier[1 + indexUnifier + 1] = indexB;
	}

	return 0;
}

// Return the unifier of two rows (naive) or -1 if not unificable
int unifier_rows(int *row_a, int *row_b, int m, int *unifier)
{
	int i, result;

	for (i = 1; i <= m; i++)
	{
		unifier[0] = 0;
		// The unifier function handles the entire row including metadata, so pointers do not need to be modified
		result = unifier_a_b(row_a, i, row_b, i + m, unifier, m, 2 * (i - 1));
		if (result != 0)
			return result;
	}

	return 0;
}

// Correct/reduce/verify the unifier
int correct_unifier(int *row_a, int *row_b, int m, int *unifier)
{
	int i, n_substitutions = 0;
	L2 *lst = (L2 *)malloc(2 * m * sizeof(L2));
	for (i = 0; i < 2 * m; i++)
	{
		lst[i] = create_L2_empty();
	}

	// For each element pair, get indexes and perform (x<-y)
	for (i = 0; i < 2 * m; i += 2)
	{
		int x = unifier[1 + i];
		int y = unifier[1 + i + 1];
		int val_y;
		int val_x;

		if (x == y && x == 0)
			continue; // Empty case

		// If any of the two elements is a repeated variable, get real index
		if (x <= m && row_a[x - 1] < 0)
			x = -row_a[x - 1];
		else if (x > m && row_b[x - m - 1] < 0)
			x = -row_b[x - m - 1] + m;
		if (y <= m && row_a[y - 1] < 0)
			y = -row_a[y - 1];
		else if (y > m && row_b[y - m - 1] < 0)
			y = -row_b[y - m - 1] + m;

		// If any of them was substituted before, get the corresponding elements
		if (lst[x - 1].count > 0)
			x = lst[x - 1].by;
		if (lst[y - 1].count > 0)
			y = lst[y - 1].by;
		if (x == y)
			continue;
		// No need to get real index again, the substitution is done for the real index

		// Get the value of x and y
		if (y > m)
			val_y = row_b[y - m - 1];
		else
			val_y = row_a[y - 1];

		if (x > m)
			val_x = row_b[x - m - 1]; 
		else
			val_x = row_a[x - 1];

		// If both constants
		if (val_x > 0 && val_y > 0 && val_x != val_y)
			return -1;
		else if (val_x > 0 && val_y > 0)
			continue;

		if (val_x > 0 && val_y == 0) // (y<-x)
		{
			// make the replacement on y
			lst[y - 1].count = 1;
			lst[y - 1].by = x;
			lst[y - 1].ind = y;

			// Update all variables replaced by y to be replaced by x
			L3 *current = lst[y - 1].head;
			while (current != NULL)
			{
				lst[current->ind - 1].by = x;
				current = current->next;
			}

			// Add the replacement list of y, and y itself, to replacement list of x
			if (lst[x - 1].head)
			{
				lst[x - 1].tail->next = create_L3(y, lst[y - 1].head);
				if (lst[y - 1].head)
					lst[x - 1].tail = lst[y - 1].tail;
			}
			else
			{
				lst[x - 1].head = create_L3(y, lst[y - 1].head);
				lst[x - 1].tail = lst[x - 1].head;
				if (lst[y - 1].head)
					lst[x - 1].tail = lst[y - 1].tail;
			}
			lst[y - 1].head = lst[y - 1].tail = NULL;
		}
		else // (x<-y)
		{
			// make the replacement on x
			lst[x - 1].count = 1;
			lst[x - 1].by = y;
			lst[x - 1].ind = x;

			// Update all variables replaced by y to be replaced by x
			L3 *current = lst[x - 1].head;
			while (current != NULL)
			{
				int current_index = current->ind;
				// printf("Altering by of index %d\n",current_index);
				lst[current_index - 1].by = y;
				current = current->next;
			}

			// Add the replacement list of y, and y itself, to replacement list of x
			if (lst[y - 1].head)
			{
				lst[y - 1].tail->next = create_L3(x, lst[x - 1].head);
				lst[y - 1].tail = lst[y - 1].tail->next;
				if (lst[x - 1].head)
					lst[y - 1].tail = lst[x - 1].tail;
			}
			else
			{
				lst[y - 1].head = create_L3(x, lst[x - 1].head);
				lst[y - 1].tail = lst[y - 1].head;
				if (lst[x - 1].head)
					lst[y - 1].tail = lst[x - 1].tail;
			}
			lst[x - 1].head = lst[x - 1].tail = NULL;
		}
	}

	int last_unifier = 0;
	// For each element, add the substitutions to the unifier
	for (i = 0; i < 2 * m; i++) 
	{
		int y = i;
		int x;
		if (lst[y].count == 0 && lst[y].head)
		{
			// Get substitutions (x <- y)
			L3 *current = lst[y].head;
			while (current != NULL)
			{
				x = current->ind;
				unifier[1 + last_unifier] = x;
				unifier[1 + last_unifier + 1] = y + 1;
				current = current->next;
				n_substitutions++;
				last_unifier += 2;
			}
		}
	}
	unifier[0] = n_substitutions;

	// Free lst
	for (i = 0; i < 2 * m; i++)
	{
		free_L2(lst[i]);
	}
	free(lst);

	return 0;
}

// Return the unifiers for two given matrices. Must have same width, and unifiers must be initialized to n0*n1 before calling function
int unifier_matrices(int *mat1, int *mat2, int n1, int n2, int *LUT1, int *LUT2, int m, int *unifiers)
{

	int i, j, unifier_size, code, last_unifier, idx_A, idx_B;
	int *unifier;

	last_unifier = 0;
	unifier_size = 1 + (2 * m) + 2;

	unifier = (int *)malloc(unifier_size * sizeof(int));

	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < n2; j++)
		{
			idx_A = LUT1[i * 2];
			idx_B = LUT2[j * 2];
			memset(unifier, 0, unifier_size * sizeof(int));
			code = unifier_rows(&mat1[idx_A * m], &mat2[idx_B * m], m, unifier);
			if (code != 0)
				continue; // Rows cannot be unified

			code = correct_unifier(&mat1[idx_A * m], &mat2[idx_B * m], m, unifier);
			if (code != 0)
				continue; // Rows cannot be unified

			unifier[1 + (2 * m)] = i;
			unifier[1 + (2 * m) + 1] = j;
			memcpy(&unifiers[last_unifier * unifier_size], unifier, unifier_size * sizeof(int));
			last_unifier++;
		}
	}

	// Free the extra space
	unifiers = realloc(unifiers, last_unifier * unifier_size * sizeof(int));
	free(unifier);
	return last_unifier;
}

// Apply unifier to just row_a, change first occurrences of variable to constant
void apply_unifier_first(int *row_a, int *row_b, int m, int *unifier)
{
	int n = unifier[0] * 2;

	int i, j;
	int x, y, val_y; // To perform x <- y

	clear(); // Clean dictionary

	// Stuff needed if y is a variable
	int length = (int)log10(2 * m) + 2;
	char *y_str = (char *)malloc(length * sizeof(char));

	for (i = 1; i < n; i += 2)
	{
		x = unifier[i];
		y = unifier[i + 1];

		if (x <= m) // Only change things in 'left' row
		{
			// Get value of y
			if (y <= m)
				val_y = row_a[y - 1];
			else
				val_y = row_b[y - m - 1];

			// y is a constant, substitute first x reference for the constant in y
			if (val_y > 0)
				row_a[x - 1] = val_y;
			else // y is a variable, so it can get tricky
			{
				// Get the string of y to see if previous appearance or not
				snprintf(y_str, length, "%d", y);
				struct nlist *entry = lookup(y_str);

				// First appearance of y, do not substitute anything, but add appearance of y linked with x
				if (entry == NULL)
					install(y_str, x);
				else // Not first appearance: need to point all x references to previous (x<-y) [effectively (x<-(-z))]
				{
					int z = entry->defn;
					row_a[x-1] = -z;
					for (j = 0; j < m; j++)
						if (row_a[j] == (-x))
							row_a[j] = -z;
				}
			}
		}
	}

	// clear(); // Improv: Is this needed?
	free(y_str);
}

// Apply unifier to just row_a, change all occurrences of variable to constant
void apply_unifier_all(int *row_a, int *row_b, int m, int *unifier)
{
	int n = unifier[0] * 2;

	int i, j;
	int x, y, val_y; // To perform x <- y

	clear(); // Clean dictionary

	// Stuff needed if y is a variable
	int length = (int)log10(2 * m) + 2;
	char *y_str = (char *)malloc(length * sizeof(char));

	for (i = 1; i < n; i += 2)
	{
		x = unifier[i];
		y = unifier[i + 1];

		if (x <= m) // Only change things in 'left' row
		{
			// Get value of y
			if (y <= m)
				val_y = row_a[y - 1]; 
			else
				val_y = row_b[y - m - 1]; 

			if (val_y > 0) // y is a constant, substitute all x references for the constant in y
			{
				row_a[x-1] = val_y;
				for (j = 0; j < m; j++)
					if (row_a[j] == (-x))
						row_a[j] = val_y;
			}
			else // y is a variable, so it can get tricky
			{
				// Get the string of y to see if previous appearance or not
				snprintf(y_str, length, "%d", y);
				struct nlist *entry = lookup(y_str);

				// First appearance of y, do not substitute anything, but add appearance of y linked with x
				if (entry == NULL)
					install(y_str, x);
				else // Not first appearance: need to point all x references to previous (x<-y) [effectively (x<-(-z))]
				{
					int z = entry->defn;
					row_a[x-1] = -z;
					for (j = 0; j < m; j++)
						if (row_a[j] == (-x))
							row_a[j] = -z;
				}
			}
		}
	}

	// clear(); // Improv: Is this needed?
	free(y_str);
}

// Checks if row1 subsums row2 or if they are qual
bool subsums_or_equal(int *row1, int *row2, int m)
{
	// If row1 and row2 do not unify, return false
	// If they unify, and row1 one variable is substituted by constant, return false
	// If they unify, and row1 variables are substituted by some row2 variable more than once, return false
	// If both rows are equal, return true
	// Any other case, return false
	
	// return false;
	int i, code;
	int unifier_size = 1+(2*m)+2;
	int *unifier = (int*)malloc(unifier_size*sizeof(int));
	if (!unifier)
    {
        fprintf(stderr, "Memory allocation failed in subsums_or_equal\n");
        exit(EXIT_FAILURE);
    }

	// 1. Check if they unify
	memset(unifier,0,unifier_size*sizeof(int));
	code = unifier_rows(row1, row2, m, unifier);
	if (code != 0) {free(unifier); return false;} // Rows cannot be unified

	code = correct_unifier(row1, row2, m, unifier);
	if (code != 0) {free(unifier); return false;} // Rows cannot be unified

	// 2. Check the unifier
	bool *used = (bool *)calloc(m, sizeof(bool));
	if (!used)
    {
        fprintf(stderr, "Memory allocation failed for used array\n");
        exit(EXIT_FAILURE);
    }

	bool equal = true;

	int x, y;
	__attribute_maybe_unused__ int val_x, val_y;
	for (i=0; i<m; i++)
	{
		x = unifier[1 + (i*2)];
		y = unifier[1 + (i*2)+1];

		int val_x = (x < m) ? row1[x] : row2[x - m];
        int val_y = (y < m) ? row1[y] : row2[y - m];

		if (val_x != val_y) equal=false;

		// If change done in row1
		if (x<m)
		{
			// If substituted by constant, does not subsum
			if (val_y > 0) {free(unifier); free(used); return false;}
			else 
			{
				// If variable of row1 is substituted multiple times with variable from row2, does not subsum
				if (used[y]) {free(unifier); free(used); return false;}
				used[y] = true;
			}
		}
	}

	free(unifier);
	free(used);
	return equal;

}

// Given the list of unifiers, this function will unify two matrices with exceptions
int matrix_intersection(int *mat1, int *mat2, int *LUT1, int *LUT2, int m, int *unifiers, int unif_count, int **result_LUT, int **result_mat)
{
	// For each pair of rows
	// 1. T1 and T2 unify, apply unifier (depending on mode?) to any of them to get NT
	// 2. For each exception E from LE1(T1) and LE2(T2), check if they unify with NT
	// 3. If so, apply unifier (depending on mode?) to E that unify, generating NE1 and NE2, ignore the rest of E
	// 4. If any NE subsumes NT, intersection for T1 and T2 is empty (possible exit)
	// 5. Create NLE taking all NE that are not subsumed by any other NE (also avoid duplicates)
	// 6. Create the NT-NLE matrix and LUT

	int i, j, k, code;
	int unifier_size = 1+(2*m)+2;
	int total_new_rows = 0;
	int num_new_rows   = 0;

	// Placeholders to perform unification operations
	int *row1     = (int*)malloc(m*sizeof(int));
	int *unifier  = (int*)malloc(unifier_size*sizeof(int));
	int *new_rows = (int*)malloc(unif_count*m*sizeof(int));
	int **new_exception_mats = (int**)malloc(unif_count*sizeof(int*));
	int *num_exception_mats  = (int*) malloc(unif_count*sizeof(int));


	if (!row1 || !unifier || !new_rows || !new_exception_mats || !num_exception_mats)
    {
        fprintf(stderr, "Initial memory allocation failed in matrix_intersection\n");
        exit(EXIT_FAILURE);
    }
	
	// For each pair of rows
	for (i=0; i<unif_count; i++)
	{
		// 1.1 Get the row index in the respective matrices
		int row_index_in_LUT1 = unifiers[unifier_size*i + unifier_size-2];
		int row_index_in_LUT2 = unifiers[unifier_size*i + unifier_size-1];


		int row_index_in_mat1 = LUT1[row_index_in_LUT1*2];
		int row_index_in_mat2 = LUT2[row_index_in_LUT2*2];

		int number_exceptions_row1 = LUT1[row_index_in_LUT1*2+1];
		int number_exceptions_row2 = LUT2[row_index_in_LUT2*2+1];

        //        row_index_in_mat1, number_exceptions_row1, row_index_in_mat2, number_exceptions_row2);

		// 1.2 Apply unifier to row1 to get NT
		memcpy(row1, &mat1[row_index_in_mat1*m], m*sizeof(int));
		apply_unifier_all(row1, &mat2[row_index_in_mat2*m], m, &unifiers[unifier_size*i]);

		int *new_exception_mat = (int*)malloc((number_exceptions_row1+number_exceptions_row2)*m*sizeof(int));
		if (!new_exception_mat)
        {
            fprintf(stderr, "Memory allocation failed for new_exception_mat\n");
            exit(EXIT_FAILURE);
        }

		int last_inserted = 0;
		for (j=row_index_in_mat1+1;j<row_index_in_mat1+1+number_exceptions_row1;j++)
		{
			memset(unifier,0,unifier_size*sizeof(int));
			code = unifier_rows(&mat1[j * m], row1, m, unifier);
			if (code != 0) continue;

			code = correct_unifier(&mat1[j * m], row1, m, unifier);
			if (code != 0) continue;

			memcpy(&new_exception_mat[last_inserted*m],&mat1[j*m],m*sizeof(int));
			apply_unifier_all(&new_exception_mat[last_inserted*m], row1, m, unifier);
			last_inserted++;
		}

		for (j=row_index_in_mat2+1; j<row_index_in_mat2+1+number_exceptions_row2; j++)
		{
			memset(unifier,0,unifier_size*sizeof(int));
			code = unifier_rows(&mat2[j * m], row1, m, unifier);
			if (code != 0) continue;

			code = correct_unifier(&mat2[j * m], row1, m, unifier);
			if (code != 0) continue;

			memcpy(&new_exception_mat[last_inserted*m], &mat2[j*m], m*sizeof(int));
			apply_unifier_all(&new_exception_mat[last_inserted*m], row1, m, unifier);
			last_inserted++;
		}

		for (j=0; j<last_inserted;j++){
			if (subsums_or_equal(&new_exception_mat[j*m],row1,m))
			{
				num_exception_mats[i] = 0;
				// break;
			}
		}

		if (num_exception_mats[i]==0) {free(new_exception_mat); continue;}
		
		bool *subsumed = (bool *)calloc(last_inserted, sizeof(bool));
        if (!subsumed)
        {
            fprintf(stderr, "Memory allocation failed for subsumed array\n");
            exit(EXIT_FAILURE);
        }

		int total_new_exceptions = last_inserted;
		printf("last_inserted: %d\n",last_inserted);
		for (j=0; j<last_inserted; j++)
		{
			if (subsumed[j]) continue;
			for (k=0; k<last_inserted; k++)
			{
				if (subsumed[k] || j==k) continue;
				if (subsums_or_equal(&new_exception_mat[j*m],&new_exception_mat[k*m],m)) {subsumed[k] = true; total_new_exceptions--;}
			}
		}

		int *NE = (int*)malloc(total_new_exceptions*m*sizeof(int));
		if (!NE)
        {
            fprintf(stderr, "Memory allocation failed for NE\n");
            exit(EXIT_FAILURE);
        }

		total_new_exceptions = 0;
		for (j=0; j<last_inserted; j++)
		{
			if (!subsumed[j]) 
			{
				memcpy(&NE[total_new_exceptions*m], &new_exception_mat[j*m], m*sizeof(int));
				total_new_exceptions++;
			}
		}

		free(new_exception_mat);
		free(subsumed);

		num_exception_mats[i] = total_new_exceptions;
		new_exception_mats[i] = NE;
		total_new_rows += 1 + total_new_exceptions;
		num_new_rows++;
		memcpy(&new_rows[i*m],row1,m*sizeof(int));
	}

	*result_LUT = (int*)malloc(num_new_rows*2*sizeof(int));
	*result_mat = (int*)malloc(total_new_rows*m*sizeof(int));
	if (!(*result_LUT) || !(*result_mat))
    {
        fprintf(stderr, "Memory allocation failed for result_LUT or result_mat\n");
        exit(EXIT_FAILURE);
    }
	
	int last_insert = 0;
	for (i=0; i<unif_count; i++)
	{
		if (num_exception_mats[i]!=0)
		{
			memcpy(&(*result_mat)[last_insert*m],     &new_rows[i*m],         m*sizeof(int));
			memcpy(&(*result_mat)[(last_insert+1)*m], new_exception_mats[i], num_exception_mats[i]*m*sizeof(int));
			(*result_LUT)[2*i]   = last_insert;
			(*result_LUT)[2*i+1] = num_exception_mats[i];
			last_insert += 1 + num_exception_mats[i];
			free(new_exception_mats[i]);
		}
	}

	free(row1);
	free(unifier);
	free(new_rows);
	free(new_exception_mats);
	free(num_exception_mats);
	return num_new_rows;
}

// --------------------- CORE END --------------------- //

int main(int argc, char *argv[])
{
	struct timespec start_total, start_reading, start_unifiers, start_unification;
	struct timespec end_total, end_reading, end_unifiers, end_unification;
	struct timespec elapsed, elapsed2;
	char *csv_file = "tests.v1/mock_test.csv";

	if (argc > 1)
		csv_file = argv[1];
	if (argc > 2)
		verbose = 1;

	int *mat1 = 0, *LUT1 = 0, n1, m1, exc1; // Matrix M1
	int *mat2 = 0, *LUT2 = 0, n2, m2, exc2; // Matrix M2
	int *mat3 = 0, *LUT3 = 0, n3, m3, exc3; // MGU	Matrix
	int m;

	clock_gettime(CLOCK_MONOTONIC_RAW, &start_total);

	// ----- read file start ----- //
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_reading);

	read_mat_file(csv_file,
				  &mat1, &LUT1, &n1, &m1, &exc1,
				  &mat2, &LUT2, &n2, &m2, &exc2,
				  &mat3, &LUT3, &n3, &m3, &exc3);

	assert(m1 == m2 && m2 == m3);
	m = m1;

	printf("Dimensions for M1 are (%d,%d,%d) and for M2 are (%d,%d,%d)\n", n1, m1, exc1, n2, m2, exc2);
	printf("MGU Dimensions: (%d,%d,%d)\n", n3, m3, exc3);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end_reading);

	if (verbose)
	{
		printf(" --- LUT for M1 --- \n");
		print_LUT(LUT1, n1);
		printf("\nValues and exceptions for M1 from %s\n", csv_file);
		// print_matrix_full(mat1,LUT1,n1,m1);
		print_matrix_clean(mat1, LUT1, n1, m1);

		printf(" --- LUT for M2 --- \n");
		print_LUT(LUT2, n2);
		printf("\nValues and exceptions for M2 from %s\n", csv_file);
		// print_matrix_full(mat2, LUT2, n2, m2);
		print_matrix_clean(mat2, LUT2, n2, m2);

		printf(" --- LUT for MGU matrix\n");
		print_LUT(LUT3, n3);
		printf("\nValues and exceptions for MGU from %s\n", csv_file);
		// print_matrix_full(mat3, LUT3, n3, m3);
		print_matrix_clean(mat3, LUT3, n3, m3);
	}
	// ----- read file end ----- //

	// ----- test unifier creation all matrix start ----- //
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_unifiers);

	int *unifiers = NULL, unifier_size = 1 + (2 * m) + 2;
	unifiers = (int *)malloc(n1 * n2 * unifier_size * sizeof(int)); // Worst case allocation
	int unif_count = unifier_matrices(mat1, mat2, n1, n2, LUT1, LUT2, m, unifiers);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end_unifiers);

	if (verbose)
		print_unifier_list(unifiers, unif_count, m);
	else
		printf("Number of unifiers: %d\n", unif_count);
	// ----- test unifier creation all matrix end ----- //

	// // ----- test unification start OLD ----- //
	// clock_gettime(CLOCK_MONOTONIC_RAW, &start_unification);
	// int *line_A = (int *)malloc(m * sizeof(int));
	// int *line_B = (int *)malloc(m * sizeof(int));
	// int *unified = (int *)malloc(unif_count * m * sizeof(int));

	// int i, idx_A, idx_B;
	// printf("Applying all unifiers . . . ");
	// for (i = 0; i < unif_count; i++)
	// {
	// 	idx_A = unifiers[i * unifier_size + unifier_size - 2];
	// 	idx_B = unifiers[i * unifier_size + unifier_size - 1];
	// 	memcpy(line_A, &mat1[idx_A * m], m * sizeof(int));
	// 	memcpy(line_B, &mat2[idx_B * m], m * sizeof(int));
	// 	apply_unifier_all(line_A, line_B, m, &unifiers[i * unifier_size]);
	// 	memcpy(&unified[i * m], line_A, m * sizeof(int));
	// }
	// clock_gettime(CLOCK_MONOTONIC_RAW, &end_unification);

	// printf("Applied all unifiers :)\n");
	// if (verbose)
	// 	print_matrix_clean_no_LUT(unified, unif_count, m);
	// ----- test unification end OLD ----- //

	// ----- test unification start NEW ----- //
	int *result_LUT = NULL;
	int *result_mat = NULL;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_unification);

	int tot_new = 0;
	tot_new = matrix_intersection(mat1, mat2, LUT1, LUT2, m, unifiers, unif_count, &result_LUT, &result_mat);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end_unification);
	printf("tot new: %d\n",tot_new);
	printf("Applied all unifiers :)\n");
	// ----- test unification end NEW ----- //

	// ----- test unification correct start ---- //
	// printf("Comparing unification results. . . ");
	// int same = compare_mgus(unified, mat2, unif_count, m);
	// if (same)
	// 	printf("Unification is correct :)\n");
	// else
	// 	printf("Unification is NOT correct :(\n");
	// ----- test unification correct end   ---- //

	clock_gettime(CLOCK_MONOTONIC_RAW, &end_total);

	printf("-------- TIME MEASUREMENTS --------\n");
	timespec_subtract(&elapsed, &end_reading, &start_reading);
	printf("Time for reading from file:    %ld.%0*ld sec\n", elapsed.tv_sec, 9, elapsed.tv_nsec);

	timespec_subtract(&elapsed, &end_unifiers, &start_unifiers);
	printf("Time for calculating unifiers: %ld.%0*ld sec\n", elapsed.tv_sec, 9, elapsed.tv_nsec);

	timespec_subtract(&elapsed2, &end_unification, &start_unification);
	printf("Time for applying unifiers:    %ld.%0*ld sec\n", elapsed2.tv_sec, 9, elapsed2.tv_nsec);

	timespec_add(&elapsed, &elapsed, &elapsed2);
	if (isatty(fileno(stdout)))
	{
		printf("Time for unification total:    \033[1;33m%ld.%0*ld sec\033[0m\n", elapsed.tv_sec, 9, elapsed.tv_nsec);
	}
	else
	{
		printf("Time for unification total:    %ld.%0*ld sec\n", elapsed.tv_sec, 9, elapsed.tv_nsec);
	}

	timespec_subtract(&elapsed, &end_total, &start_total);
	printf("Total time:                    %ld.%0*ld sec\n", elapsed.tv_sec, 9, elapsed.tv_nsec);

	// free(line_A);
	// free(line_B);
	// free(unified);
	free(result_LUT);
	free(result_mat);
	free(mat1);
	free(mat2);
	free(mat3);
	return 0;
}
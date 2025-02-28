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
	int n_elems = n * (1 + m);
	int i;

	for (i = 0; i < n_elems; i++)
	{
		if (my_mgu[i] != other_mgu[i])
		{
			int row = i / (1 + m);
			int col = i % (1 + m);
			printf("\n");
			printf("My result: ");
			print_mat_line(&my_mgu[row * (1 + m)], m);
			printf("Original:  ");
			print_mat_line(&other_mgu[row * (1 + m)], m);
			printf("Different elements at row %d col %d: %d != %d\n", row, col, my_mgu[i], other_mgu[i]);
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

		col = 0; // Start indexing columns from 1
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
				int index = (row * line_len) + col;
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
	if (indexA >= m)
		real_indexA = real_indexA - m;
	if (indexB >= m)
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

// Correct/reduce/verify the unifier, unifier pointer must be pointing to row_a's unifier (for now)
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
		if (x < m && row_a[x] < 0)
			x = -row_a[x];
		else if (x >= m && row_b[x - m] < 0)
			x = -row_b[x - m] + m;
		if (y < m && row_a[y] < 0)
			y = -row_a[y];
		else if (y >= m && row_b[y - m] < 0)
			y = -row_b[y - m] + m;

		// If any of them was substituted before, get the corresponding elements
		if (lst[x - 1].count > 0)
			x = lst[x - 1].by;
		if (lst[y - 1].count > 0)
			y = lst[y - 1].by;
		if (x == y)
			continue;
		// No need to get real index again, the substitution is done for the real index

		// Get the value of x and y
		if (y >= m)
			val_y = row_b[y - m];
		else
			val_y = row_a[y];

		if (x >= m)
			val_x = row_b[x - m];
		else
			val_x = row_a[x];

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

		if (x < m) // Only change things in 'left' row
		{
			// Get value of y
			if (y < m)
				val_y = row_a[y];
			else
				val_y = row_b[y - m];

			// y is a constant, substitute first x reference for the constant in y
			if (val_y > 0)
				row_a[x] = val_y;
			else // y is a variable, so it can get tricky
			{
				// Get the string ov y to see if previous appearance or not
				snprintf(y_str, length, "%d", y);
				struct nlist *entry = lookup(y_str);

				// First appearance of y, do not substitute anything, but add appearance of y linked with x
				if (entry == NULL)
					install(y_str, x);
				else // Not first appearance: need to point all x references to previous (x<-y) [effectively (x<-(-z))]
				{
					int z = entry->defn;
					row_a[x] = -z;
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

		if (x < m) // Only change things in 'left' row
		{
			// Get value of y
			if (y < m)
				val_y = row_a[y];
			else
				val_y = row_b[y - m];

			if (val_y > 0) // y is a constant, substitute all x references for the constant in y
			{
				row_a[x] = val_y;
				for (j = 0; j < m; j++)
					if (row_a[j] == (-x))
						row_a[j] = val_y;
			}
			else // y is a variable, so it can get tricky
			{
				// Get the string ov y to see if previous appearance or not
				snprintf(y_str, length, "%d", y);
				struct nlist *entry = lookup(y_str);

				// First appearance of y, do not substitute anything, but add appearance of y linked with x
				if (entry == NULL)
					install(y_str, x);
				else // Not first appearance: need to point all x references to previous (x<-y) [effectively (x<-(-z))]
				{
					int z = entry->defn;
					row_a[x] = -z;
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

// Improv: could be using Trie or other structure to optimize this search, for now just doing it raw
// Returns exceptions without duplicated lines; now exceptions is updated via pointer-to-pointer
int delete_duplicates(int **exceptions, int n, int m)
{
	int i, j, k;
	int *clean_exceptions = (int *)malloc(n * m * sizeof(int));
	bool duplicated, equal;
	int last_clean_line = 0;

	for (i = 0; i < n; i++)
	{
		j = i + 1;
		duplicated = false;
		while (j < n && duplicated == false)
		{
			k = 0;
			equal = true;
			while (k < m && equal == true)
			{
				if ((*exceptions)[i * m + k] != (*exceptions)[j * m + k])
					equal = false;
				k++;
			}
			duplicated = equal;
			j++;
		}
		if (!duplicated)
		{
			memcpy(&clean_exceptions[last_clean_line * m], &(*exceptions)[i * m], m * sizeof(int));
			last_clean_line++;
		}
	}
	*exceptions = realloc(*exceptions, last_clean_line * m * sizeof(int)); // Sizing down, so I skip the check to see if realloc did good (:$)
	memcpy(*exceptions, clean_exceptions, last_clean_line * m * sizeof(int));
	free(clean_exceptions);
	return last_clean_line;
}

// Checks if any exception subsums any other exception or the new row, return 0 if no subsumes, 1 otherwise
int subsums(int *exceptions, int n_exc, int *new_row, int m)
{
	int i, j, k;
	int unif_size = (1 + 2 * m + 2) * sizeof(int);
	int *unifier = (int *)malloc(unif_size);
	int code;
	bool delete, subsums, subsums_left, subsums_right;

	// For each exception row
	i = 0;
	delete = false;
	while (i < n_exc && !delete)
	{
		// Check exception against new row
		memset(unifier, 0, unif_size);
		code = unifier_rows(&exceptions[i * m], new_row, m, unifier);
		if (code != 0)
		{
			delete = true;
			continue;
		}

		code = correct_unifier(&exceptions[i * m], new_row, m, unifier);
		if (code != 0)
		{
			delete = true;
			continue;
		}

		// Traverse unifier to see if exception subsums new unified row
		k = 0;
		subsums = true;
		while (k < unifier[0] && subsums)
		{
			// If the unifier performs a change on exceptions[i], esceptions[i] does not subsum new_row
			if (unifier[k * 2 + 1] < m)
			{
				subsums = false;
				continue;
			}

			// It could be that all variables of exceptions[i] are substituted for distinct variables of new_row, in that case exc does not subsum new_row either
			// TODO : Also check alternate case

			k++;
		}

		delete = subsums;

		// Check exception agains all other exceptions
		j = i + 1;
		while (j < n_exc && !delete)
		{
			code = unifier_rows(&exceptions[i * m], &exceptions[j * m], m, unifier);
			if (code != 0)
			{
				delete = true;
				continue;
			}

			code = correct_unifier(&exceptions[i * m], &exceptions[j * m], m, unifier);
			if (code != 0)
			{
				delete = true;
				continue;
			}

			// Traverse unifier to see which exception subsums which
			k = 0;
			subsums_left = subsums_right = true;
			while (k < unifier[0] && (subsums_left || subsums_right))
			{
				// Check easy condition for subsumation
				if (unifier[k * 2 + 1] < m)
				{
					subsums_left = false;
					continue;
				}
				if (unifier[k * 2 + 1] >= m)
				{
					subsums_right = false;
					continue;
				}

				// TODO : Also check alternate case where all variables on exc[i] are assigned distinct variables of exc[j]

				k++;
			}
			if (subsums_left)
				delete = true;
			else if (subsums_right)
			{
				// TODO : Remove exception[i] from the list, continue
			}

			j++;
		}
		i++;
	}

	if (delete)
		return 1;
	else
		return 0;
}

// Given the list of unifiers, this function will unify two matrices with exceptions
int unify(int *mat1, int *mat2, int *LUT1, int *LUT2, int m, int *unifiers, int unif_count, int mode, int **result_LUT, int **result_mat)
{
	int i, idx_LUT_A, idx_LUT_B, idx_A, idx_B;
	int n_exc_A, n_exc_B, n_exc;
	int delete, num_saved, total_exceptions;
	int unifier_size = 1 + (2 * m) + 2;
	int total_lines = 0;

	bool *which_save = (bool *)malloc(unif_count * sizeof(bool));

	// Create 'empty' lines for unification operation
	int *line_A = (int *)malloc(m * sizeof(int));
	int *line_B = (int *)malloc(m * sizeof(int));

	// Create structures for the unified lines and a new exception matrix for each new unified line
	int *unified = (int *)malloc(unif_count * m * sizeof(int));
	int *exception_count = (int *)malloc(unif_count * sizeof(int));
	int **exception_mats = (int **)malloc(unif_count * sizeof(int *));

	for (i = 0; i < unif_count; i++)
	{
		// 1. Apply unifier (first or all, depending on 'mode')
		idx_LUT_A = unifiers[i * unifier_size + unifier_size - 2];
		idx_LUT_B = unifiers[i * unifier_size + unifier_size - 1];

		idx_A = LUT1[idx_LUT_A * 2];
		idx_B = LUT2[idx_LUT_B * 2];

		memcpy(line_A, &mat1[idx_A * m], m * sizeof(int));
		memcpy(line_B, &mat2[idx_B * m], m * sizeof(int));
		if (mode == 0)
			apply_unifier_first(line_A, line_B, m, &unifiers[i * unifier_size]);
		else
			apply_unifier_all(line_A, line_B, m, &unifiers[i * unifier_size]);
		memcpy(&unified[i * m], line_A, m * sizeof(int));

		// 2. Append exceptions of both rows to the new unified row
		n_exc_A = LUT1[idx_LUT_A * 2 + 1];
		n_exc_B = LUT2[idx_LUT_B * 2 + 1];
		n_exc = n_exc_A + n_exc_B;

		exception_mats[i] = (int *)malloc(n_exc * m * sizeof(int));
		memcpy(exception_mats[i], &mat1[(idx_A + 1) * m], n_exc_A * m * sizeof(int));
		memcpy(&exception_mats[i][n_exc_A * m], &mat2[(idx_B + 1) * m], n_exc_B * m * sizeof(int));

		// 3. Delete duplicated lines
		n_exc = delete_duplicates(&exception_mats[i], n_exc, m);

		// 4. Check if any of the new exceptions subsums the unified row, or if they subsums between themselves
		delete = subsums(exception_mats[i], n_exc, &unified[i * m], m);

		// 5. If not, save results
		if (!delete)
		{
			which_save[i] = true;
			exception_count[i] = n_exc;
			num_saved++;
			total_exceptions += n_exc;
		}
		else
		{
			which_save[i] = false;
			free(exception_mats[i]);
		}
	}

	// At the end, traverse which_save, unified and exception_mats to calculate MGU LUT and create the result matrix
	(*result_mat) = (int *)malloc((total_exceptions + num_saved) * m * sizeof(int));
	(*result_LUT) = (int *)malloc(num_saved * 2 * sizeof(int));
	int last_row_index = 0;
	int last_index = 0;
	int n_exceptions;

	for (i = 0; i < unif_count; i++)
	{
		if (which_save[i])
		{
			// Update MGU LUT
			n_exceptions = exception_count[i];
			(*result_LUT)[2 * last_index] = last_row_index;
			(*result_LUT)[2 * last_index + 1] = n_exceptions;

			// Update MGU matrix
			memcpy(&(*result_mat)[last_row_index * m], &unified[i * m], m * sizeof(int));
			memcpy(&(*result_mat)[(last_row_index + 1) * m], exception_mats[i], n_exceptions * m * sizeof(int));

			free(exception_mats[i]);
			last_row_index += 1 + n_exceptions;
			last_index++;
		}
	}

	free(line_A);
	free(line_B);
	free(unified);
	free(exception_count);
	free(exception_mats);
	free(which_save);

	// Return the new unif_count
	return num_saved;
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
		// print_LUT(LUT1, n1);
		printf("\nValues and exceptions for M1 from %s\n", csv_file);
		// print_matrix_full(mat1,LUT1,n1,m1);
		print_matrix_clean(mat1, LUT1, n1, m1);

		printf(" --- LUT for M2 --- \n");
		// print_LUT(LUT2, n2);
		printf("\nValues and exceptions for M2 from %s\n", csv_file);
		// print_matrix_full(mat2, LUT2, n2, m2);
		print_matrix_clean(mat2, LUT2, n2, m2);

		printf(" --- LUT for MGU matrix\n");
		// print_LUT(LUT3, n3);
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

	// ----- test unification start ----- //
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_unification);
	int *line_A = (int *)malloc(m * sizeof(int));
	int *line_B = (int *)malloc(m * sizeof(int));
	int *unified = (int *)malloc(unif_count * m * sizeof(int));

	int i, idx_A, idx_B;
	printf("Applying all unifiers . . . ");
	for (i = 0; i < unif_count; i++)
	{
		idx_A = unifiers[i * unifier_size + unifier_size - 2];
		idx_B = unifiers[i * unifier_size + unifier_size - 1];
		memcpy(line_A, &mat1[idx_A * m], m * sizeof(int));
		memcpy(line_B, &mat2[idx_B * m], m * sizeof(int));
		apply_unifier_all(line_A, line_B, m, &unifiers[i * unifier_size]);
		memcpy(&unified[i * m], line_A, m * sizeof(int));
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end_unification);

	printf("Applied all unifiers :)\n");
	if (verbose)
		print_matrix_clean_no_LUT(unified, unif_count, m);
	// ----- test unification end ----- //

	// ----- test unification correct start ---- //
	printf("Comparing unification results. . . ");
	int same = compare_mgus(unified, mat2, unif_count, m);
	if (same)
		printf("Unification is correct :)\n");
	else
		printf("Unification is NOT correct :(\n");
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

	free(line_A);
	free(line_B);
	free(unified);
	free(mat1);
	free(mat2);
	free(mat3);
	return 0;
}
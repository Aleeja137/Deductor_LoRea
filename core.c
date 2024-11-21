#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "structures.h"
#include "dictionary.h"

#define N 50
#define M 10

struct nlist *dict;


// Leer matrices desde fichero
void read_mat_file(char input_file[], L1** mat){
    FILE *stream;
    char * line = NULL;
    char* tok;
    size_t len=0, last_mat_element = 0;
    ssize_t read;
    int value_const;
    int last_const;

    // Abrir fichero y verificar acceso
    stream = fopen(input_file, "r");
    if (stream == NULL) {fprintf(stderr, "Error opening file %s, exiting\n",input_file); exit(EXIT_FAILURE);}
    else printf("File %s opened correctly :) \n", input_file); // Check

    // setrlimit // Improv full: Se podría hacer esto para limitar posibles errores en el fichero de lectura y que no explote la memoria (?)
    // Procesar fichero línea a línea
    while((read = getline(&line, &len, stream)) != -1) 
    {
        // Saltarme líneas de comentario // Revise full: Igual me interesa leer estas líneas para sacar info de la matrx (dimensiones?)
        if (line[0] == '%') continue;

        // Procesar línea token a token
        for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
        {
            // Si es constante
            if (isdigit(tok[0]))
            {
                // Crear nodo L1 con ese valor int
                // Improv full: viendo que las constantes se repiten mucho, podría usar el diccionario para que todas las mismas constantes apunten al mismo L1 (más espacio de diccionario, mayor tiempo procesamiento de lectura de matriz, pero menos memoria de programa). Para ver cómo, ver el siguiente else
            }
            else
            {
                // If the variable does not exist in the dictionary, create a L1 with 0 for value. Add entry to dictionary with the string with the memory address (https://stackoverflow.com/questions/73711419/how-to-convert-variables-address-to-string-variable-in-c)
                // If the variable exists in the dictionary, point to the L1, taking the address from the dictionary (https://stackoverflow.com/questions/15081863/string-to-address-c)
            }

            if ((dict = lookup("E")) != NULL)
            printf("%s\n",tok); // Check
        }

        printf("Retrieved line of length %zu:\n", read); // Check
        // printf("%s", line); // Check
    }

    fclose(stream);
    if (line) free(line);
}


// Leer matriz unificada desde cero y sus unificadores
void read_unif_mat_file();
// Procesar ficheros, sólo uno, un directorio, etc
void process_files();

int main (int argc, char *argv[]){
    L1** mat = constr_mat_vacia(N,M);
    read_mat_file("benchmark/test00.csv",mat);
    free_L1_mat(N,M,mat);
    printf("Main Completed, argument count %d, program name %s\n",argc, argv[0]);
    return 0;
}
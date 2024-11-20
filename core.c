#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "structures.h"
#include "dictionary.h"

#define N 50
#define M 10

struct nlist *dict;


// Leer matrices desde fichero
void read_mat_file(char input_file[]){
    FILE *stream;
    char * line = NULL;
    const char* tok;
    size_t len=0;
    ssize_t read;

    // Abrir fichero y verificar acceso
    stream = fopen(input_file, "r");
    if (stream == NULL) {fprintf(stderr, "Error opening file %s, exiting\n",input_file); exit(EXIT_FAILURE);}
    else printf("File %s opened correctly :) \n", input_file); // Check

    // setrlimit // Improv full: Se podría hacer esto para limitar posibles errores en el fichero de lectura y que no explote la memoria (?)
    // Procesar fichero línea a línea
    while((read = getline(&line, &len, stream)) != -1) 
    {
        // Skip line if comment // Revise full: Igual me interesa leer estas líneas para sacar info de la matrx (dimensiones?)
        if (line[0] == '%') continue;

        // Process line token by token
        for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
        {
            // If it is a constant
            if (islower(tok[0]))
            {

            }
            else
            {

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
    read_mat_file("benchmark/test00.csv");
    L1** mat = constr_mat_vacia(N,M);
    free_L1_mat(N,M,mat);
    printf("Main Completed, argument count %d, program name %s\n",argc, argv[0]);
    return 0;
}
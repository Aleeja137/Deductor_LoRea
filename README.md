# Título proyecto  
Info entidades involucradas, personas, correos (?).  
Info proyecto, etc.  
## Ficheros  
### core.c  
Funciones para manejar el cómputo de matrices, unificadores, etc.  
### structures.c  
Funciones para manejar las estructuras de datos definidas para este proyecto (L1, L2, L3).  
### limpiar.sh  
`limpiar.sh` quita comentarios extra del código fuente. El objetivo es poder dejar comentarios durante el desarrollo a los que poder volver más tarde, y poder tene runa versión 'limpia' manteniendo comentarios generales pero eliminando comentarios/líneas que sólo son útiles durante el desarrollo. Se podría conseguir un functionamiento similar con `#ifdef DEBUG` o algo por el estilo, pero prefiero esta manera. La leyenda de los comentarios es la siguiente:  

**// Check** son líneas usadas para probar el funcionamiento de ciertas cosas, se pueden borrar o comentar cuando todo funcione. `limpiar.sh` borra la línea entera.  
**// Improv** son sugerencias para hacer el código más rápido/eficiente/robusto. `limpiar.sh` borra el comentario y el resto desde el comentario hasta el fin de línea.  
**// Improv full** son sugerencias para hacer el código más rápido/eficiente/robusto. `limpiar.sh` borra la línea entera.  
**// Revise** son cosas que no sé si son correctas, revisar en un futuro por si acaso. `limpiar.sh` borra el comentario y el resto desde el comentario hasta el fin de línea.  
**// Revise full** son cosas que no sé si son correctas, revisar en un futuro por si acaso. `limpiar.sh` borra la línea entera.  

Funcionamiento de `limpiar.sh`:  
`./limpiar.sh original.c limpio.c`  

Ejemplo original:  
```
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
    }
    ...
}
```

Ejemplo limpio:  
```
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

    // Procesar fichero línea a línea
    while((read = getline(&line, &len, stream)) != -1) 
    {
        if (line[0] == '%') continue;
    }
    ...
}
```
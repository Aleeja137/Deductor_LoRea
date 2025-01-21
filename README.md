# Título proyecto  
Info entidades involucradas, personas, correos (?).  
Info proyecto, etc.  

## Compilación  
`gcc -o c core.c -Wall -Wextra -g -O0 structures.c -lm`  

## Uso  
`./c secondExamples/test0002.csv` siendo test0002.csv cualquier fichero de la carpeta secondExamples.  

## Uso prolog  
`swipl -f get_times.pl -g "main('Files/AGT004+1.p/test000X.pl'), halt"`  

Para la comprobación de memoria con valgrind:  
`valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./c secondExamples/test0001.csv`  
Dejará el resultado en valgrind-out.txt en la misma carpeta que el ejecutable.   

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

### change_variables.sh  
Este script procesa los ficheros csv de una carpeta y los deja en otra. Los procesa sustituyendo los carácteres en minúscula por valores integers, manteniendo consistencia (si 'a' se cambia por 1, lo hace en todo el fichero). Procesa los carácteres en mayúscula haciendo que cada variable dentro de una línea tome el mismo string, y las variables entre filas, aunque inicialmente sean el mismo carácter, se cambian por strings distintos.  

por comodidad, y por ahora, no procesa los resultados de unificar y los unificadores incluídos en los benchmarks.  
  
Uso:  
'./change_variables.sh <carpeta_benchmakrs_original> <carpeta_benchmakrs_cambiado>'  

Ejemplo original:  
```  
%%% BEGIN: Matrix M1 %%%
a,b,a,a,a,b,C,a,a,a
a,a,a,b,A,a,a,A,b,A
B,a,b,a,A,b,B,b,a,b
b,a,C,a,b,A,b,b,b,a
%%% END: Matrix M1 %%%
%%% BEGIN: Matrix M2 %%%
a,E,a,a,a,a,b,E,b,b
G,a,A,b,b,a,a,E,a,G
F,G,E,E,a,a,a,a,G,E
E,A,b,a,a,a,H,a,a,b
%%% END: Matrix M2 %%%
```  

Ejemplo cambiado:  
```  
%%% BEGIN: Matrix M1 %%%
1,2,1,1,1,2,C1,1,1,1
1,1,1,2,A1,1,1,A1,2,A1
B1,1,2,1,A2,2,B1,2,1,2
2,1,C2,1,2,A3,2,2,2,1
%%% END: Matrix M1 %%%
%%% BEGIN: Matrix M2 %%%
1,E1,1,1,1,1,2,E1,2,2
G1,1,A4,2,2,1,1,E2,1,G1
F1,G2,E3,E3,1,1,1,1,G2,E3
E4,A5,2,1,1,1,H1,1,1,2
```  
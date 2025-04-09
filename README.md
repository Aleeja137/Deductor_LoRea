# Deductor LoRea
Este proyecto aspira a implementar un deductor lógico de primer orden con contraints de igualdad en C, y después optimizarlo y acelerarlo en GPUs.  
Trabajo desarrollado en colaboración por:  
- [LoRea EHU](https://www.ehu.eus/es/web/lorea/web-gunea)  
- [ISG EHU](http://www.sc.ehu.es/ccwbayes/)  

## Branch info  
Functionalites:  
    -   
    -       
  
  
## Compilación  
`gcc -o c core.c -Wall -Wextra -g -O0 structures.c -lm`  
`gcc -o c core.c -Wall -Wextra -O2 structures.c -lm`  

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


```  
^\s*(?!.*//.*//).*// Check.*  
```  

V0 - Sólo unificar
    - Arreglar versión 1 (low priority)
    - Comprobar con representación interna si unifico bien
        - Arreglar unificación -> (indices negativos fuera de rango: NO FUNCIONA BIEN)
        - Tener dos versiones de unificación, una en la que cambiamos la primera instancia de variable<-constante, y otra donde se cambian todas (seguramente esta se use para comprobar unificación correcta en implementación interna). 
        - Hacer pruebas para ver el impacto de cambiar todas las apariciones 
        - Optimizar el código
        - Arreglar leaks

V1 - Unifica con excepciones

V2 - Unifica con excepciones y nombres de columnas aplanados

V3 - Lo mismo pero nombres de columnas sin aplanar y con los términos de aplanamiento en profundidad
    - 3:[1,2:[1:[1]],_]

tests.v0 son simplemente unificación, sin esquema aplanado
tests.v1 son con y sin excepciones, con esquema aplanado

A mejorar:  
    - Añadir un valor en mgu schema que me diga si los dos esquemas son iguales (me ahorro el tratamiento en prepare_unified)  
    - Añadir un valor en mgu schema que me diga si la intersección de los esquemas (n) es 0, para hacer la operación bastante más rápido (copia-pega)  
    - Añadir vector de booleanos para (mapping_L y) mapping_R en mgu_schema para saber qué columnas no están en común y hay que hacer append (en prepare_united)  
    - Añadir un array de offsets de los valores que no están en común entre mgu y M2, así en lugar de ir haciendo append 1 a 1 puedo hacer append de varios (en prepare_united)  
    - Poner un diccionario dedicado a variables y otro a constantes, el de variables se puede limpiar línea a línea para que la búsqueda sea más corta, y el de constantes será más corto también imagino
    - Mirar si es subconjunto, easier copy
    - Meter memory handling, arreglar memory leaks, añadir documentación y limpiar/mejorar/actualizar comentarios
    - Meter const/unsigned donde sea necesario, usar size_t también para recorrer posiciones de memoria si son necesarios (preguntar a Jose sino)
    - Podríamos meter una función que simplifique el unificador (var<-var que no se repiten más)


Siguientes pasos:  
    - Leer nuevo formato con excepcioness  
    - Aplicar la lógica de unificación con excepciones  
    - La comparación para ver si es correcto o no podría ser únicamente el número de filas nuevas (ya que es muy muy sensible y puede ser sufi), ya que con dos variables X e Y, que no toman valor y se repiten, las dos opciones (x<-y / y<-x) son equivalentes pero las detecta como mal.
        - Hay otro caso donde la unificación está 'bien' y detecta mal, que es cuando hacemos append de las clumnas en r_B que no son en común al final, y prolog suele hacer el append en 'orden'; aunque esto se podría arreglar con un 'orden' a la hora de comparar

    - Voy a tener que generar muy buena documentación cuando acabe para el siguiente que trabaje en esto (o para mí si sigo trabajando en esto)


TODO
// 1. Read file fuera (DONE)
// 2. unifier_matrices tiuene que trabajar con operand blocks y el mgu_schema del result_block (DONE)
// 3. Cherry pick los commits the master para la unificación con excepciones (DONE)
    // 3.1 Comprobar en godbolt que la función subsums funciona correctamente 
// 4. Comprobar los resultados del result block creado con el result block leído (DONE(?))
    // 4.1 Hace falta comprobar las columnas en orden, mucho cuidado con el mgu_schema
// 5. Comprobar y arreglar memory leaks (DONE)
// 6. Añadir documentación
// 7. Actualizar/quitar todos los comentarios y líneas de check
// 8. Subir al main, documentación del README a punto
// 9. Repasar el código para posibles optimizaciones (preguntar)
// 10. Pedir a Jose cuenta en el cluster nuevo (DONE)

- Implementar lógica con _-_
- Implementar lógica de comparación de resultados
- Leer nuevo formato de archivos (columnas para comprbar subsums en excepciones)
- Testear todo y acabar la lista de arriba
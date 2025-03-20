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
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

Sequencia con esquemas aplanados:
1. Leer matrix_schema para cada matriz a la vez que los datos
2. Sacar columns de mgu_schema según los comunes de matrix_schema de M1 y M2
3. Usando los índices de mapping según mgu_schema, sacar los unificadores para cada pareja de líneas, también unif_count
4. Con unif_count y m_M1+m_M2-m_MGU, reservar memoria para MGU. 
    4.1 Copiar mapping M1 a MGU, rellenar con mapping_L2-mapping_M1
5. Para cada unificador
    5.1 Copiar r_M1 a MGU
    5.2 Aplicar el unificador en MGU
    5.3 Copiar Lr_M2 a MGU


    
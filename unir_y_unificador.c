
update_pointers (x,y) 
// X e Y son dos variables L1 struct, y se deben 
// actualizar para hacer la sustitución x<-y

    // Si la info de variable está vacía, crearla
    if (y.L2 == null)
        y.L2 = new L2(0,null,null,y)
    
    // Añadir x al final de la lista de variable info
    if (y.L2.tail == null)
        y.L2.head = new L3(x,null)
        y.L2.tail = y.L2.head
    else
        y.L2.tail.next = new L3(x,null)
        y.L2.tail = y.L2.tail.next
    
    // Guardar entry y añadir variable info de x al 
    // final de variable info de y
    entry = y.L2.tail
    if (x.L2 != null)
        y.L2.tail.next = x.L2.head
        y.L2.tail = x.L2.tail

    // Recorrer todas las variables y que su variable info 
    // apunte a variable info de y
    current = entry
    while (current != null)
        // Liberar el variable info de x (todas las variables
        // sustituídas por x usan ese variable info)
        if (current.next == null)
            free(current.L1.L2)
        current.L1.L2 = y.L2
        current = current.next

update_unifier(omega, a, b) 
// A y B son dos structs L1 que pueden ser constantes o variables

    // A es constante (o variable inicializada a constante)
    if (a.value > 0 && b.value > 0 && a.value!=b.value) return E
    if (a.value > 0 && b.value <=0) return update_unifier(omega,b,a)

    // A es variable sin inicializar
    if (a.value == 0 && b.value > 0) 
        a.value = b.value
        if (a.L2 == null) a.L2 = new L2(b.value,null,null,a)
        else a.L2.value = b.value
        return omega.append(a<-b)
    if (a.value == 0 && b.value <= 0) 
        update_pointers(a,b)
        return omega.append(a<-b)
    
    // A es variable inicializada a otra variable C
    if (a.value < 0) return update_modifier(omega,a.L2.me,b)
        
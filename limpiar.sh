#!/bin/bash

# Verificar argumentos
if [ $# -ne 2 ]; then
    echo "Uso: $0 <fichero_original> <fichero_limpio>"
    exit 1
fi

# Asignar argumentos a variables
input_file="$1"
output_file="$2"

# Verificar que el archivo original existe
if [ ! -f "$input_file" ]; then
    echo "Error: el archivo '$input_file' no existe."
    exit 1
fi

# Eliminar el archivo de salida si ya existe
if [ -f "$output_file" ]; then
    rm "$output_file"
fi

# Procesar el archivo línea por línea
while IFS= read -r line || [ -n "$line" ]; do
    if [[ "$line" == *"// Check"* ]]; then
        # Omitir completamente la línea
        continue
    elif [[ "$line" == *"// Improv full"* ]]; then
        # Omitir completamente la línea
        continue
    elif [[ "$line" == *"// Improv"* ]]; then
        # Cortar el comentario y el resto desde // Improv
        line=$(echo "$line" | sed -E 's#// Improv.*##')
    elif [[ "$line" == *"// Revise full"* ]]; then
        # Omitir completamente la línea
        continue
    elif [[ "$line" == *"// Revise"* ]]; then
        # Cortar el comentario y el resto desde // Revise
        line=$(echo "$line" | sed -E 's#// Revise.*##')
    fi
    # Escribir la línea procesada en el fichero de salida
    echo "$line" >> "$output_file"
done < "$input_file"

echo "Archivo procesado guardado en '$output_file'."

#!/bin/bash

# Verificar argumentos
if [ $# -ne 2 ]; then
    echo "Uso: $0 <carpeta_benchmakrs_original> <carpeta_benchmakrs_cambiado>"
    exit 1
fi

# Asignar argumentos a variables
benchmark_src="$1"
benchmark_dst="$2"

mkdir -p "$benchmark_dst"

# Recorrer los archivos de la carpeta
for file in "$benchmark_src"/*.csv; do

    file_stripped=${file##*/}
    last_const=1
    > "$benchmark_dst"/"$file_stripped"

    ## Para cada archivo, tener un diccionario de constantes
    declare -A constantes_fichero

    ## Para cada archivo, tener un diccionario de variables usadas ya
    declare -A variables_fichero

    # Para cada archivo, recorrerlo línea a línea
    while IFS= read -r line; do
        # Las matrices ya se han procesado, parar si se alcanza el resultado o '&' or 'MGU'
        if [[ "$line" == *"&"* || "$line" == *"MGU"* ]]; then
            break
        fi
        unset variables_linea
        declare -A variables_linea
        touch "$benchmark_dst"/"$file_stripped"

        # Para cada línea, si no es línea de comentario o vacía, recorrer elemento a elemento
        if [[ -n "$line" && ! "$line" =~ "BEGIN" && ! "$line" =~ "END" ]]; then
            IFS=',' read -ra tokens <<< "$line"

            for i in "${!tokens[@]}"; do
                tok="${tokens[i]}"

                # Si es minúscula, sustituirlo por un valor entero
                if [[ "$tok" =~ [a-z] ]]; then
                    # Si ese carácter ya existe en el diccionario, sustituirlo por integer
                    if [[ -n "${constantes_fichero[$tok]}" ]]; then
                        tokens[i]="${constantes_fichero[$tok]}" 
                    # Si ese carácter no existe en el diccionario, añadirlo y sustituirlo
                    else
                        tokens[i]=$(("$last_const"))
                        constantes_fichero["$tok"]=$(("$last_const"))
                        last_const=$((last_const + 1))
                    fi
                # Si es mayúscula, sustituírlo por variable
                elif [[ "$tok" =~ [A-Z] ]]; then 
                    # Si esa variable es nueva, añadirlo a los diccionarios
                    if [[ -z "${variables_fichero[$tok]}" ]]; then
                        variables_fichero[$tok]="1"
                        variables_linea[$tok]="$tok""1"
                        tokens[i]=$tok"1" 
                    # Si esa variable es nueva en la línea, pero no en el fichero, añadir un número
                    elif [[ -n "${variables_fichero[$tok]}" && -z "${variables_linea[$tok]}" ]]; then
                        num_rep="${variables_fichero[$tok]}"
                        num_rep=$(($num_rep + 1))
                        variables_fichero[$tok]="$num_rep"
                        variables_linea["$tok"]="$tok""$num_rep"
                        tokens[i]="${variables_linea["$tok"]}"
                    # Si esa variable es usada ya en la línea, sustituir a secas
                    elif [[ -n "${variables_fichero[$tok]}" && -n "${variables_linea[$tok]}" ]]; then
                        tokens[i]="${variables_linea[$tok]}"
                    fi
                fi
            done
            modified_line=$(IFS=','; echo "${tokens[*]}")  
        # Si es línea de comentario o vacía, copiarla directamente
        else 
            modified_line="$line"    
        fi

        # Write to the output file
        echo "$modified_line" >> "$benchmark_dst"/"$file_stripped"
    done < "$file"
done
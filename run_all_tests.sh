#!/bin/bash

# Check if the directory is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <directory> [WE|NE]"
    exit 1
fi

directory="$1"
mode="$2"

# Determine the pattern based on the flag
if [ "$mode" == "NE" ]; then
    pattern="*.no_exceptions.csv"
else
    pattern="*.csv"
fi

# Iterate over all matching CSV files in the given directory and its subdirectories
find "$directory" -type f -name "$pattern" | while read -r file; do
    # Skip "no_exceptions" files if in WE mode
    if [ "$mode" != "NE" ] && [[ "$file" == *.no_exceptions.csv ]]; then
        continue
    fi
    echo "========= <<< $file >>> ========="
    ./c "$file"
    echo -e "\n\n"
done

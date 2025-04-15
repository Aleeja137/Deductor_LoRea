#!/bin/bash

FOLDER="/home/javier/MassiveUnification/Tests/matrices/COM123+1"
CMD="./c"

gcc -o c core.c -Wall -Wextra -O3 structures.c COM_hash.c -lm
# Start time
start_time=$(date +%s)

# Loop through all M1 files
for fileM1 in "$FOLDER"/*M1.csv; do
    base=$(basename "$fileM1" M1.csv)
    fileM2="$FOLDER/${base}M2.csv"
    fileM3="$FOLDER/${base}M3.csv"

    # Check if all three files exist
    if [[ -f "$fileM2" && -f "$fileM3" ]]; then
        echo "Running: $CMD $fileM1 $fileM2 $fileM3"
        $CMD "$fileM1" "$fileM2" "$fileM3"
    else
        echo "Skipping: $base — missing M2 or M3"
    fi
done

# End time
end_time=$(date +%s)
total_time=$((end_time - start_time))

echo "✅ Total time: $total_time seconds"

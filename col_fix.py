import os
import re
import argparse
from pathlib import Path

def process_matrix_file(input_file_path, output_folder_path):
    with open(input_file_path, 'r') as f:
        lines = f.readlines()

    output_lines = []
    i = 0
    while i < len(lines):
        line = lines[i]
        output_lines.append(line)
        
        # Detect start of a matrix subset
        if line.strip().startswith("% BEGIN: Matrix subset"):
            # Read until we get the mapping line (starts with numbers and dashes)
            while i < len(lines) and not re.match(r"^\d+-\d+(,\d+-\d+|,_-_|_-_,)*", lines[i].strip()):
                i += 1
                output_lines.append(lines[i])
            
            mapping_line = lines[i].strip()
            mappings = mapping_line.split(',')
            fix_indices = [index for index, m in enumerate(mappings) if m.strip() == '_-_']
            fix_labels = [f"FIX_{j+1}" for j in range(len(fix_indices))]

            # Read column headers (they follow mapping)
            i += 1
            output_lines.append(lines[i])  # column header line

            i += 1  # move to the data rows
            while i < len(lines):
                line = lines[i]

                if line.strip().startswith('% END: Matrix subset') or line.strip().startswith('% BEGIN: Matrix subset'):
                    break  # End of subset
                elif re.match(r"^Rows \d+-\d+: not unifiable", line.strip()):
                    output_lines.append(line)
                elif re.match(r"^Rows \d+-\d+:", line.strip()):  # unified row
                    prefix, row_data = line.strip().split(":", 1)
                    columns = [col.strip() for col in row_data.strip().split(',')]
                    for fix_idx, fix_label in zip(fix_indices, fix_labels):
                        if fix_idx < len(columns):
                            columns[fix_idx+1] = fix_label
                    new_line = f"{prefix}: {','.join(columns)}\n"
                    output_lines.append(new_line)
                else:
                    output_lines.append(line)
                i += 1
            continue  # already incremented i
        i += 1

    # Ensure output folder exists
    os.makedirs(output_folder_path, exist_ok=True)
    output_file_path = os.path.join(output_folder_path, Path(input_file_path).name)

    with open(output_file_path, 'w') as f:
        f.writelines(output_lines)

    print(f"File processed and saved to: {output_file_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process matrix subset files and replace _-_ columns with FIX_N labels.")
    parser.add_argument("input_file", help="Path to the input file")
    parser.add_argument("output_folder", help="Folder to save the processed output")

    args = parser.parse_args()
    process_matrix_file(args.input_file, args.output_folder)

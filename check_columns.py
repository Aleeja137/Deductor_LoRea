import csv
import re
import sys
import os


def check_csv_file(path):
    # Keywords to skip in data lines
    skip_keywords = [":", "not unifiable", "subsumed by exception", "Unifier"]

    # Stack to manage nested subsets with types: ('matrix', cols) or ('exception', cols)
    stack = []

    # Regex to match any BEGIN header with parentheses
    begin_re = re.compile(r"%+\s*BEGIN:\s*(Matrix subset\S*|Exception subset\S*)\s*\(([^)]*)\)")
    # Regex to match any End/END line marking subset end
    end_re = re.compile(r"%+\s*(?:End|END):")

    with open(path, newline='', encoding='utf-8') as f:
        for lineno, raw_line in enumerate(f, 1):
            line = raw_line.rstrip("\n")

            # Detect BEGIN lines and push (type, expected cols)
            m_begin = begin_re.match(line)
            if m_begin:
                subset_type = 'matrix' if m_begin.group(1).startswith('Matrix') else 'exception'
                parts = [p.strip() for p in m_begin.group(2).split(',')]
                try:
                    cols = int(parts[-1])
                except ValueError:
                    cols = None
                stack.append((subset_type, cols))
                continue

            # Detect END/End lines and pop
            if end_re.match(line):
                if stack:
                    stack.pop()
                continue

            # Skip comment lines
            if line.lstrip().startswith('%'):
                continue

            # Skip lines containing any skip keyword
            if any(kw in line for kw in skip_keywords):
                continue

            # Determine context and expected/original cols
            expected_cols = None
            original_c = None
            # Check for active exception
            for t, c in reversed(stack):
                if t == 'exception':
                    expected_cols = c
                    break
            else:
                # Fallback to matrix context
                for t, c in reversed(stack):
                    if t == 'matrix':
                        original_c = c
                        expected_cols = c + 1
                        break

            # If no relevant context, skip
            if expected_cols is None:
                continue

            # Parse CSV row and count columns
            row = next(csv.reader([line]))
            actual_cols = len(row)

            # Allow numeric sequence rows 1..original_c in matrix context
            if original_c is not None and actual_cols == original_c:
                # check if row is ['1','2',...,'original_c']
                seq = [str(i) for i in range(1, original_c + 1)]
                if row == seq:
                    continue

            # Report mismatches
            if actual_cols != expected_cols:
                print(f"{path}:{lineno}: expected {expected_cols} columns, got {actual_cols} -> {line}")


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <csv-file-or-directory>")
        sys.exit(1)

    target = sys.argv[1]
    if os.path.isdir(target):
        # Walk directory and process all .csv files
        for root, dirs, files in os.walk(target):
            for fname in files:
                if fname.lower().endswith('.csv'):
                    fullpath = os.path.join(root, fname)
                    print("Checking",fullpath, "------------------------")
                    check_csv_file(fullpath)
    elif os.path.isfile(target):
        print("Checking",target, "------------------------")
        check_csv_file(target)
    else:
        print(f"Error: {target} is not a file or directory.")
        sys.exit(1)

if __name__ == '__main__':
    main()


# print("Checking",target, "------------------------")

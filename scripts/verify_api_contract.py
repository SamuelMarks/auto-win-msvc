import sys
import json
import os
import re

def find_header(repo_root, header_name):
    # Search all include/ directories for the header
    for root, dirs, files in os.walk(repo_root):
        if 'include' in root.split(os.sep):
            for file in files:
                rel_path = os.path.relpath(os.path.join(root, file), root)
                # Some headers have folders, e.g. sys/wait.h
                # Check if the suffix matches
                full_path = os.path.join(root, file)
                if full_path.replace('\\', '/').endswith('/' + header_name) or file == header_name:
                    return full_path
    return None

def verify_contract(repo_root, json_spec_path):
    try:
        with open(json_spec_path, 'r', encoding='utf-8') as f:
            spec = json.load(f)
    except Exception as e:
        print(f"Failed to read {json_spec_path}: {e}")
        return False

    all_ok = True

    for header_name, symbols in spec.items():
        header_path = find_header(repo_root, header_name)
        if not header_path:
            print(f"Error: Header {header_name} not found in the repository.")
            all_ok = False
            continue

        with open(header_path, 'r', encoding='utf-8') as f:
            content = f.read()

        for symbol in symbols.keys():
            # We look for the symbol as a whole word
            if not re.search(r'\b' + re.escape(symbol) + r'\b', content):
                print(f"Error: Symbol '{symbol}' not found in {header_path}")
                all_ok = False

    return all_ok

if __name__ == '__main__':
    all_ok = True
    repo_root = '.'
    for arg in sys.argv[1:]:
        if arg.endswith('.json') and 'specs' in arg.split(os.sep):
            if not verify_contract(repo_root, arg):
                all_ok = False
    if not all_ok:
        sys.exit(1)

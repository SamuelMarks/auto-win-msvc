import sys
import subprocess
import os

def check_file(filepath):
    # Find the directory containing the file
    dir_name = os.path.dirname(filepath)
    # The module is typically the parent of src
    module_dir = os.path.dirname(dir_name) if os.path.basename(dir_name) == 'src' else dir_name

    # Collect all include directories in the repository
    repo_root = '.'
    include_dirs = [f'-I{os.path.join(module_dir, "include")}', '-Iinclude', '-I.']
    for root, dirs, files in os.walk(repo_root):
        if 'include' in root.split(os.sep):
            # Only add the top-level 'include' directories of modules
            if os.path.basename(root) == 'include':
                # Don't add module_dir again
                if root != os.path.join(module_dir, 'include'):
                    include_dirs.append(f'-I{root}')

    cmd = [
        'gcc', '-std=c90', '-pedantic', '-Werror', '-fsyntax-only',
        '-D_POSIX_C_SOURCE=200809L', '-D_XOPEN_SOURCE=700', filepath
    ] + include_dirs


    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error checking {filepath}:\n{result.stderr}")
            return False
        return True
    except Exception as e:
        print(f"Failed to execute gcc on {filepath}: {e}")
        return False

if __name__ == '__main__':
    all_ok = True
    for f in sys.argv[1:]:
        if f.endswith('.c') and 'tests' not in f.split(os.sep) and not os.path.basename(f).startswith('test_'):
            if not check_file(f):
                all_ok = False
    if not all_ok:
        sys.exit(1)

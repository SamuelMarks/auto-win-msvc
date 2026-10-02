import sys
import re

def check_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False

    ok = True

    # Check for exactly one pair of #ifdef __cplusplus blocks per public header
    if filepath.endswith('.h'):
        cplusplus_blocks = len(re.findall(r'#ifdef\s+__cplusplus', content))
        if cplusplus_blocks != 2:
            print(f"{filepath}: Found {cplusplus_blocks} `#ifdef __cplusplus` blocks. Expected exactly 2 (one opening, one closing).")
            ok = False

    # Check for exactly one /* clang-format off */ block per file
    clang_format_off = content.count('/* clang-format off */')
    clang_format_on = content.count('/* clang-format on */')

    if clang_format_off > 1 or clang_format_on > 1:
        print(f"{filepath}: Found {clang_format_off} `/* clang-format off */` blocks. Expected maximum 1.")
        ok = False

    if clang_format_off != clang_format_on:
        print(f"{filepath}: Mismatched `/* clang-format off */` ({clang_format_off}) and `/* clang-format on */` ({clang_format_on}) blocks.")
        ok = False

    return ok

if __name__ == '__main__':
    all_ok = True
    for f in sys.argv[1:]:
        if not check_file(f):
            all_ok = False
    if not all_ok:
        sys.exit(1)

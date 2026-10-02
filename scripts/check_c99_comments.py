import sys

def check_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False

    in_string = False
    in_char = False
    in_block = False
    escape = False
    i = 0
    while i < len(content):
        if escape:
            escape = False
            i += 1
            continue
        if content[i] == '\\':
            escape = True
            i += 1
            continue

        if not in_string and not in_char and not in_block:
            if content[i:i+2] == '/*':
                in_block = True
                i += 2
                continue
            if content[i:i+2] == '//':
                # Allow // inside http:// inside string, but we are outside strings here.
                # Just ban it entirely.
                line_no = content[:i].count('\n') + 1
                print(f"{filepath}:{line_no}: C99-style comment `//` found.")
                return False
            if content[i] == '"':
                in_string = True
                i += 1
                continue
            if content[i] == "'":
                in_char = True
                i += 1
                continue

        elif in_block:
            if content[i:i+2] == '*/':
                in_block = False
                i += 2
                continue
        elif in_string:
            if content[i] == '"':
                in_string = False
        elif in_char:
            if content[i] == "'":
                in_char = False

        i += 1

    return True

if __name__ == '__main__':
    all_ok = True
    for f in sys.argv[1:]:
        if not check_file(f):
            all_ok = False
    if not all_ok:
        sys.exit(1)

import os, glob

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    if '__cplusplus' in content:
        return

    # find the last #include or the #define of the header guard
    lines = content.splitlines()
    insert_start = 0
    insert_end = len(lines)

    for i, line in enumerate(lines):
        if line.startswith('#include '):
            insert_start = i + 1

    for i in range(len(lines)-1, -1, -1):
        if lines[i].startswith('#endif'):
            insert_end = i
            break

    if insert_start == 0:
        for i, line in enumerate(lines):
            if line.startswith('#define '):
                insert_start = i + 1
                break

    wrapper_start = ['#ifdef __cplusplus', 'extern \"C\" {', '#endif /* __cplusplus */']
    wrapper_end = ['#ifdef __cplusplus', '}', '#endif /* __cplusplus */']

    new_lines = lines[:insert_start] + wrapper_start + lines[insert_start:insert_end] + wrapper_end + lines[insert_end:]

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write('\n'.join(new_lines) + '\n')

for root, dirs, files in os.walk('.'):
    if 'build' in root or 'test' in root or 'temp_redis' in root or 'rewritten_src' in root or 'valkey_src' in root:
        continue
    for file in files:
        if file.endswith('.h'):
            process_file(os.path.join(root, file))

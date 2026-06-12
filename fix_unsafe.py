import os

def replace_unsafe(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        c = f.read()

    lines = c.split('\n')
    new_lines = []

    for line in lines:
        if 'strcpy(' in line:
            # simple regex or string replacement for standard strcpy(dest, src);
            if 'strcpy(buf, "CON");' in line:
                new_lines.append(line.replace('strcpy(buf, "CON");', '#if defined(_MSC_VER)\n    strcpy_s(buf, 4, "CON");\n#else\n    strcpy(buf, "CON");\n#endif'))
                continue
            if 'strcpy(search_path, name);' in line:
                new_lines.append(line.replace('strcpy(search_path, name);', '#if defined(_MSC_VER)\n  strcpy_s(search_path, sizeof(search_path), name);\n#else\n  strcpy(search_path, name);\n#endif'))
                continue
            if 'strcpy(dirp->name, name);' in line:
                new_lines.append(line.replace('strcpy(dirp->name, name);', '#if defined(_MSC_VER)\n  strcpy_s(dirp->name, sizeof(dirp->name), name);\n#else\n  strcpy(dirp->name, name);\n#endif'))
                continue
            if 'strcpy(search_path, dirp->name);' in line:
                new_lines.append(line.replace('strcpy(search_path, dirp->name);', '#if defined(_MSC_VER)\n  strcpy_s(search_path, sizeof(search_path), dirp->name);\n#else\n  strcpy(search_path, dirp->name);\n#endif'))
                continue
        new_lines.append(line)

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write('\n'.join(new_lines))

replace_unsafe(r'C:\Users\samue\repos\auto-win-msvc\posix-core\src\posix-core.c')
replace_unsafe(r'C:\Users\samue\repos\auto-win-msvc\posix-dirent\src\posix-dirent.c')

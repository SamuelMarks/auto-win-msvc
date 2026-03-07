import re

with open('src/posix-dlfcn.c', 'r') as f:
    text = f.read()

text = text.replace('#include <minwindef.h>\\n#include <errhandlingapi.h>', '#include <minwindef.h>\\n#include <windef.h>\\n#include <errhandlingapi.h>')

with open('src/posix-dlfcn.c', 'w') as f:
    f.write(text)

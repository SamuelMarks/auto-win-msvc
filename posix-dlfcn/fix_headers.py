import re

with open('src/posix-dlfcn.c', 'r') as f:
    text = f.read()

# Fix headers block
headers_old = '''#if defined(_MSC_VER) && _MSC_VER <= 1400
#include <windef.h>
#else
#include <minwindef.h>
#endif
#include <windef.h>
#include <winbase.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>

#include <memoryapi.h>'''

headers_new = '''#if defined(_MSC_VER) && _MSC_VER <= 1400
#pragma warning(push)
#pragma warning(disable: 4201 4214)
#include <windef.h>
#include <winbase.h>
#pragma warning(pop)
#else
#include <minwindef.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <winbase.h>
#include <memoryapi.h>
#endif'''

text = text.replace(headers_old, headers_new)

with open('src/posix-dlfcn.c', 'w') as f:
    f.write(text)


c = open('tests/test.c').read()
c = c.replace('#ifdef _MSC_VER\n#include <crtdbg.h>', '#ifdef _MSC_VER\n#pragma warning(disable: 4127)\n#include <crtdbg.h>')
open('tests/test.c', 'w').write(c)


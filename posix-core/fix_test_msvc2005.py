
c = open('tests/test.c').read()
c = c.replace('#include <stdint.h>', '#if _MSC_VER >= 1600\n#include <stdint.h>\n#else\n#include <stddef.h>\n#endif')
open('tests/test.c', 'w').write(c)


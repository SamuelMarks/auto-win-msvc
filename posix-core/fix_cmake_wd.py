
c = open('CMakeLists.txt').read()
c = c.replace('/D_CRT_NONSTDC_NO_WARNINGS', '/wd4127 /D_CRT_NONSTDC_NO_WARNINGS')
open('CMakeLists.txt', 'w').write(c)


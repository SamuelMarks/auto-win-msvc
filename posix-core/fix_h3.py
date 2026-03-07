
import re
c = open('include/posix-core.h').read()

# Restore uid_t and gid_t for MinGW
c = c.replace('#endif /* _MSC_VER */', '''#endif /* _MSC_VER */
#if defined(__MINGW32__) || defined(__MINGW64__)
#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif
#endif''')

# Fix sleep macros by using inline functions
c = re.sub(r'#define sleep\(x\).*?\n', '', c)
c = re.sub(r'#define usleep\(x\).*?\n', '', c)

sleep_funcs = '''
static __inline unsigned int posix_core_sleep(unsigned int seconds) {
    Sleep(seconds * 1000);
    return 0;
}
#define sleep posix_core_sleep

static __inline int posix_core_usleep(unsigned int usec) {
    Sleep(usec / 1000);
    return 0;
}
#define usleep posix_core_usleep
'''
c = c.replace('#ifndef usleep\n#endif', '#ifndef usleep\n' + sleep_funcs + '#endif')

open('include/posix-core.h', 'w').write(c)


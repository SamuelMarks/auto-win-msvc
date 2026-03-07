
import re
c = open('include/posix-core.h').read()

c = re.sub(
    r'#if defined\(_WIN32\) && !defined\(__CYGWIN__\)\n#ifndef _SSIZE_T_DEFINED.*?typedef int gid_t;\n#endif',
    r'''#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER)
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#endif

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#ifndef _MODE_T_DEFINED
#define _MODE_T_DEFINED
typedef unsigned int mode_t;
#endif

#ifndef _OFF_T_DEFINED
#define _OFF_T_DEFINED
typedef long _off_t;
typedef _off_t off_t;
#endif

#ifndef _USECONDS_T_DEFINED
#define _USECONDS_T_DEFINED
typedef unsigned int useconds_t;
#endif

#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif
#endif /* _MSC_VER */''', c, flags=re.DOTALL)

c = c.replace('#define sleep(x) ((void)Sleep((x) * 1000), 0)', '#define sleep(x) (Sleep((x) * 1000) ? 0 : 0)')
c = c.replace('#define usleep(x) ((void)Sleep((x) / 1000), 0)', '#define usleep(x) (Sleep((x) / 1000) ? 0 : 0)')

open('include/posix-core.h', 'w').write(c)

c2 = open('src/posix-core.c').read()
c2 = c2.replace('#if defined(_WIN32) && !defined(__CYGWIN__)\n#pragma warning(disable: 4100)', '#ifdef _MSC_VER\n#pragma warning(disable: 4100)')
open('src/posix-core.c', 'w').write(c2)


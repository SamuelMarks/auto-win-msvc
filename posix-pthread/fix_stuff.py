with open('include/posix-pthread.h', 'r') as f:
    h = f.read()

# Instead of removing all typedefs, let's keep the missing ones:
replacement = """#else
#if !defined(__APPLE__) && !defined(__linux__)
typedef int pthread_key_t;
typedef int pthread_t;
typedef int pthread_mutex_t;
typedef int pthread_cond_t;
typedef int pthread_rwlock_t;
typedef int pthread_once_t;
typedef int pthread_attr_t;
typedef int pthread_mutexattr_t;
typedef int pthread_condattr_t;
typedef int pthread_rwlockattr_t;
#endif

/* These are often missing on macOS */
#ifndef _PTHREAD_BARRIER_T_DEFINED
#define _PTHREAD_BARRIER_T_DEFINED
typedef int pthread_barrier_t;
typedef int pthread_barrierattr_t;
#endif

#ifndef _PTHREAD_SPINLOCK_T_DEFINED
#define _PTHREAD_SPINLOCK_T_DEFINED
typedef int pthread_spinlock_t;
#endif

#ifndef _SEM_T_DEFINED
#define _SEM_T_DEFINED
typedef int sem_t;
#endif
#endif"""

import re
h = re.sub(r'#else\n#if !defined\(__APPLE__\) && !defined\(__linux__\)\n.*?\n#endif\n#endif', replacement, h, flags=re.DOTALL)

with open('include/posix-pthread.h', 'w') as f:
    f.write(h)


import json

with open('mappings.json', 'r') as f:
    data = json.load(f)

from generate_code import sigs

out = []
out.append("""/* posix-pthread.h - Strict C89 Header */
#ifndef POSIX_PTHREAD_H
#define POSIX_PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <time.h>

#if defined(_WIN32) && !defined(POSIX_PTHREAD_NO_WINDOWS_H)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
typedef DWORD pthread_key_t;
typedef HANDLE pthread_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef SRWLOCK pthread_rwlock_t;
typedef INIT_ONCE pthread_once_t;
typedef HANDLE sem_t;
typedef struct { void *ptr; } pthread_attr_t;
typedef struct { void *ptr; } pthread_mutexattr_t;
typedef struct { void *ptr; } pthread_condattr_t;
typedef struct { void *ptr; } pthread_rwlockattr_t;
typedef struct { void *ptr; } pthread_barrier_t;
typedef struct { void *ptr; } pthread_barrierattr_t;
typedef struct { void *ptr; } pthread_spinlock_t;
#else
typedef int pthread_key_t;
typedef int pthread_t;
typedef int pthread_mutex_t;
typedef int pthread_cond_t;
typedef int pthread_rwlock_t;
typedef int pthread_once_t;
typedef int sem_t;
typedef int pthread_attr_t;
typedef int pthread_mutexattr_t;
typedef int pthread_condattr_t;
typedef int pthread_rwlockattr_t;
typedef int pthread_barrier_t;
typedef int pthread_barrierattr_t;
typedef int pthread_spinlock_t;
#endif

#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif

#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
typedef int sigset_t;
#endif

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#ifndef _CLOCKID_T_DEFINED
#define _CLOCKID_T_DEFINED
typedef int clockid_t;
#endif

struct sched_param {
    int sched_priority;
};

#define PTHREAD_MUTEX_INITIALIZER {(void*)-1,-1,0,0,0,0} /* mock for windows */
#define PTHREAD_COND_INITIALIZER {0}
#define PTHREAD_RWLOCK_INITIALIZER {0}
#define PTHREAD_ONCE_INIT {0}

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCELED ((void *)-1)

/* Functions */
""")

for func in data['mappings']['functions']:
    name = func['posix']
    sig = sigs[name]
    out.append(f"/**")
    out.append(f" * @brief {name}")
    out.append(f" * ")
    out.append(f" * Polyfill for {name}.")
    out.append(f" */")
    out.append(f"{sig};")

out.append("""
#ifdef __cplusplus
}
#endif

#endif /* POSIX_PTHREAD_H */
""")

with open('include/posix-pthread.h', 'w') as f:
    f.write('\n'.join(out))


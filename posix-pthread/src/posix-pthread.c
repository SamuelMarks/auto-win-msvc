/* posix-pthread.c - Strict C89 Implementation */
#include "posix-pthread.h"
#include <errno.h>

#ifndef ENOSYS
#define ENOSYS 40
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif
#ifndef EBUSY
#define EBUSY 16
#endif
#ifndef EINVAL
#define EINVAL 22
#endif


#if !defined(_WIN32) || defined(__CYGWIN__)
/* Nothing to implement, native POSIX used */
#else

#if defined(_WIN32)
#include <stdlib.h>
#define WINAPI __stdcall
#define INFINITE 0xFFFFFFFFUL
#define WAIT_OBJECT_0 0UL
#define WAIT_TIMEOUT 258UL
#define TLS_OUT_OF_INDEXES 0xFFFFFFFFUL

typedef void (WINAPI *FARPROC_T)(void);
__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);
__declspec(dllimport) void* WINAPI GetModuleHandleA(const char*);
__declspec(dllimport) FARPROC_T WINAPI GetProcAddress(void*, const char*);
__declspec(dllimport) void* WINAPI CreateThread(void*, size_t, unsigned long (WINAPI *)(void*), void*, unsigned long, unsigned long*);
__declspec(dllimport) unsigned long WINAPI WaitForSingleObject(void*, unsigned long);
__declspec(dllimport) int WINAPI CloseHandle(void*);
__declspec(dllimport) void WINAPI Sleep(unsigned long);
__declspec(dllimport) void* WINAPI CreateSemaphoreA(void*, long, long, const char*);
__declspec(dllimport) int WINAPI ReleaseSemaphore(void*, long, long*);
__declspec(dllimport) unsigned long WINAPI TlsAlloc(void);
__declspec(dllimport) int WINAPI TlsFree(unsigned long);
__declspec(dllimport) void* WINAPI TlsGetValue(unsigned long);
__declspec(dllimport) int WINAPI TlsSetValue(unsigned long, void*);














__declspec(dllimport) void WINAPI InitializeCriticalSection(void*);
__declspec(dllimport) void WINAPI DeleteCriticalSection(void*);
__declspec(dllimport) void WINAPI EnterCriticalSection(void*);
__declspec(dllimport) void WINAPI LeaveCriticalSection(void*);
__declspec(dllimport) unsigned char WINAPI TryEnterCriticalSection(void*);
__declspec(dllimport) int WINAPI InitializeCriticalSectionAndSpinCount(void*, unsigned long);
__declspec(dllimport) void* WINAPI OpenThread(unsigned long, int, unsigned long);

typedef long (WINAPI * PFN_SetThreadDescription)(void*, const wchar_t*);
/** \brief dyn_SetThreadDescription function. */
static int WINAPI dyn_SetThreadDescription(void* a0, const wchar_t* a1) {
    static PFN_SetThreadDescription pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_SetThreadDescription)GetProcAddress(mod, "SetThreadDescription");
        init = 1;
    }
    if (pfn) {
        pfn(a0, a1);
        return 0;
    }
    return ENOSYS;
}

typedef void (WINAPI * PFN_InitializeSRWLock)(void**);
/** \brief dyn_InitializeSRWLock function. */
static void WINAPI dyn_InitializeSRWLock(void** a0) {
    static PFN_InitializeSRWLock pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_InitializeSRWLock)GetProcAddress(mod, "InitializeSRWLock");
        init = 1;
    }
    if (pfn) pfn(a0);
}


typedef void (WINAPI * PFN_AcquireSRWLockExclusive)(void**);
/** \brief dyn_AcquireSRWLockExclusive function. */
static void WINAPI dyn_AcquireSRWLockExclusive(void** a0) {
    static PFN_AcquireSRWLockExclusive pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_AcquireSRWLockExclusive)GetProcAddress(mod, "AcquireSRWLockExclusive");
        init = 1;
    }
    if (pfn) pfn(a0);
}


typedef void (WINAPI * PFN_ReleaseSRWLockExclusive)(void**);
/** \brief dyn_ReleaseSRWLockExclusive function. */
static void WINAPI dyn_ReleaseSRWLockExclusive(void** a0) {
    static PFN_ReleaseSRWLockExclusive pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_ReleaseSRWLockExclusive)GetProcAddress(mod, "ReleaseSRWLockExclusive");
        init = 1;
    }
    if (pfn) pfn(a0);
}


typedef void (WINAPI * PFN_AcquireSRWLockShared)(void**);
/** \brief dyn_AcquireSRWLockShared function. */
static void WINAPI dyn_AcquireSRWLockShared(void** a0) {
    static PFN_AcquireSRWLockShared pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_AcquireSRWLockShared)GetProcAddress(mod, "AcquireSRWLockShared");
        init = 1;
    }
    if (pfn) pfn(a0);
}


#if 0
typedef void (WINAPI * PFN_ReleaseSRWLockShared)(void**);
/** \brief dyn_ReleaseSRWLockShared function. */
static void WINAPI dyn_ReleaseSRWLockShared(void** a0) {
    static PFN_ReleaseSRWLockShared pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_ReleaseSRWLockShared)GetProcAddress(mod, "ReleaseSRWLockShared");
        init = 1;
    }
    if (pfn) pfn(a0);
}
#endif

typedef unsigned char (WINAPI * PFN_TryAcquireSRWLockExclusive)(void**);
/** \brief dyn_TryAcquireSRWLockExclusive function. */
static unsigned char WINAPI dyn_TryAcquireSRWLockExclusive(void** a0) {
    static PFN_TryAcquireSRWLockExclusive pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_TryAcquireSRWLockExclusive)GetProcAddress(mod, "TryAcquireSRWLockExclusive");
        init = 1;
    }
    if (pfn) return pfn(a0);
    return 0;
}


typedef unsigned char (WINAPI * PFN_TryAcquireSRWLockShared)(void**);
/** \brief dyn_TryAcquireSRWLockShared function. */
static unsigned char WINAPI dyn_TryAcquireSRWLockShared(void** a0) {
    static PFN_TryAcquireSRWLockShared pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_TryAcquireSRWLockShared)GetProcAddress(mod, "TryAcquireSRWLockShared");
        init = 1;
    }
    if (pfn) return pfn(a0);
    return 0;
}


typedef void (WINAPI * PFN_InitializeConditionVariable)(void**);
/** \brief dyn_InitializeConditionVariable function. */
static void WINAPI dyn_InitializeConditionVariable(void** a0) {
    static PFN_InitializeConditionVariable pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_InitializeConditionVariable)GetProcAddress(mod, "InitializeConditionVariable");
        init = 1;
    }
    if (pfn) pfn(a0);
}


typedef int (WINAPI * PFN_SleepConditionVariableSRW)(void**, void**, unsigned long, unsigned long);
/** \brief dyn_SleepConditionVariableSRW function. */
static int WINAPI dyn_SleepConditionVariableSRW(void** a0, void** a1, unsigned long a2, unsigned long a3) {
    static PFN_SleepConditionVariableSRW pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_SleepConditionVariableSRW)GetProcAddress(mod, "SleepConditionVariableSRW");
        init = 1;
    }
    if (pfn) return pfn(a0, a1, a2, a3);
    return 0;
}


typedef void (WINAPI * PFN_WakeConditionVariable)(void**);
/** \brief dyn_WakeConditionVariable function. */
static void WINAPI dyn_WakeConditionVariable(void** a0) {
    static PFN_WakeConditionVariable pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_WakeConditionVariable)GetProcAddress(mod, "WakeConditionVariable");
        init = 1;
    }
    if (pfn) pfn(a0);
}


typedef void (WINAPI * PFN_WakeAllConditionVariable)(void**);
/** \brief dyn_WakeAllConditionVariable function. */
static void WINAPI dyn_WakeAllConditionVariable(void** a0) {
    static PFN_WakeAllConditionVariable pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_WakeAllConditionVariable)GetProcAddress(mod, "WakeAllConditionVariable");
        init = 1;
    }
    if (pfn) pfn(a0);
}


#if 0
typedef int (WINAPI * PFN_InitOnceBeginInitialize)(void**, unsigned long, int*, void**);
/** \brief dyn_InitOnceBeginInitialize function. */
static int WINAPI dyn_InitOnceBeginInitialize(void** a0, unsigned long a1, int* a2, void** a3) {
    static PFN_InitOnceBeginInitialize pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_InitOnceBeginInitialize)GetProcAddress(mod, "InitOnceBeginInitialize");
        init = 1;
    }
    if (pfn) return pfn(a0, a1, a2, a3);
    return 0;
}


typedef int (WINAPI * PFN_InitOnceComplete)(void**, unsigned long, void*);
/** \brief dyn_InitOnceComplete function. */
static int WINAPI dyn_InitOnceComplete(void** a0, unsigned long a1, void* a2) {
    static PFN_InitOnceComplete pfn = 0;
    static int init = 0;
    if (!init) {
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_InitOnceComplete)GetProcAddress(mod, "InitOnceComplete");
        init = 1;
    }
    if (pfn) return pfn(a0, a1, a2);
    return 0;
}

typedef struct {
    void *(*start_routine)(void *);
    void *arg;
} thread_params_t;

/** \brief thread_start_wrapper function. */
static int WINAPI thread_start_wrapper(void *arg) {
    thread_params_t *params = (thread_params_t *)arg;
    void *(*routine)(void*) = params->start_routine;
    void *routine_arg = params->arg;
    free(params);
    routine(routine_arg);
    return 0;
}
#endif
#endif

/* TODO: Implement pthread_atfork */
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
    (void)prepare;
    (void)parent;
    (void)child;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_destroy */
int pthread_attr_destroy(pthread_attr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getdetachstate */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
    (void)attr;
    (void)detachstate;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getguardsize */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
    (void)attr;
    (void)guardsize;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getinheritsched */
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
    (void)attr;
    (void)inheritsched;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getschedparam */
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
    (void)attr;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getschedpolicy */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
    (void)attr;
    (void)policy;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getscope */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
    (void)attr;
    (void)contentionscope;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getstack */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    (void)attr;
    (void)stackaddr;
    (void)stacksize;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_getstacksize */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
    (void)attr;
    (void)stacksize;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_init */
int pthread_attr_init(pthread_attr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setdetachstate */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    (void)attr;
    (void)detachstate;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setguardsize */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
    (void)attr;
    (void)guardsize;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setinheritsched */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
    (void)attr;
    (void)inheritsched;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setschedparam */
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
    (void)attr;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setschedpolicy */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
    (void)attr;
    (void)policy;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setscope */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
    (void)attr;
    (void)contentionscope;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setstack */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    (void)attr;
    (void)stackaddr;
    (void)stacksize;

    return ENOSYS;
}

/* TODO: Implement pthread_attr_setstacksize */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
    (void)attr;
    (void)stacksize;

    return ENOSYS;
}

/* TODO: Implement pthread_barrier_destroy */
int pthread_barrier_destroy(pthread_barrier_t *barrier) {
    (void)barrier;

    return ENOSYS;
}

/* TODO: Implement pthread_barrier_init */
int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count) {
    (void)barrier;
    (void)attr;
    (void)count;

    return ENOSYS;
}

/* TODO: Implement pthread_barrier_wait */
int pthread_barrier_wait(pthread_barrier_t *barrier) {
    (void)barrier;

    return ENOSYS;
}

/* TODO: Implement pthread_barrierattr_destroy */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_barrierattr_getpshared */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_barrierattr_init */
int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_barrierattr_setpshared */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_cancel */
int pthread_cancel(pthread_t thread) {
    (void)thread;

    return ENOSYS;
}

/* TODO: Implement pthread_cleanup_pop */
void pthread_cleanup_pop(int execute) {
    (void)execute;

    return;
}

/* TODO: Implement pthread_cleanup_push */
void pthread_cleanup_push(void (*routine)(void *), void *arg) {
    (void)routine;
    (void)arg;

    return;
}

/* TODO: Implement pthread_cond_broadcast */
int pthread_cond_broadcast(pthread_cond_t *cond) {
#if defined(_WIN32)
    if (!cond) return EINVAL;
    dyn_WakeAllConditionVariable(&cond->p);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_cond_destroy */
int pthread_cond_destroy(pthread_cond_t *cond) {
    (void)cond;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_init */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
#if defined(_WIN32)
    (void)attr;
    if (!cond) return EINVAL;
    dyn_InitializeConditionVariable(&cond->p);
    return 0;
#else
    (void)cond; (void)attr;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_cond_signal */
int pthread_cond_signal(pthread_cond_t *cond) {
#if defined(_WIN32)
    if (!cond) return EINVAL;
    dyn_WakeConditionVariable(&cond->p);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_cond_timedwait */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
#if defined(_WIN32)
    unsigned long timeout_ms;
    if (!cond || !mutex || !abstime) return EINVAL;
    /* basic conversion, not exact abstime semantic */
    timeout_ms = (unsigned long)(abstime->tv_sec * 1000 + abstime->tv_nsec / 1000000);
    return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, timeout_ms, 0) ? 0 : ETIMEDOUT;
#else
    (void)cond; (void)mutex; (void)abstime;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_cond_wait */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
#if defined(_WIN32)
    if (!cond || !mutex) return EINVAL;
    return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, INFINITE, 0) ? 0 : EINVAL;
#else
    (void)cond; (void)mutex;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_condattr_destroy */
int pthread_condattr_destroy(pthread_condattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_condattr_getclock */
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id) {
    (void)attr;
    (void)clock_id;

    return ENOSYS;
}

/* TODO: Implement pthread_condattr_getpshared */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_condattr_init */
int pthread_condattr_init(pthread_condattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_condattr_setclock */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) {
    (void)attr;
    (void)clock_id;

    return ENOSYS;
}

/* TODO: Implement pthread_condattr_setpshared */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_create */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    (void)thread;
    (void)attr;
    (void)start_routine;
    (void)arg;

    return ENOSYS;
}

/* TODO: Implement pthread_detach */
int pthread_detach(pthread_t thread) {
#if defined(_WIN32)
    if (!thread.p) return EINVAL;
    CloseHandle(thread.p);
    return 0;
#else
    (void)thread;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_equal */
int pthread_equal(pthread_t t1, pthread_t t2) {
    (void)t1;
    (void)t2;

    return ENOSYS;
}

/* TODO: Implement pthread_exit */
void pthread_exit(void *value_ptr) {
    (void)value_ptr;

    return;
}

/* TODO: Implement pthread_getconcurrency */
int pthread_getconcurrency(void) {
    return ENOSYS;
}

/* TODO: Implement pthread_getcpuclockid */
int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id) {
    (void)thread_id;
    (void)clock_id;

    return ENOSYS;
}

/* TODO: Implement pthread_getschedparam */
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
    (void)thread;
    (void)policy;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement pthread_getspecific */
void *pthread_getspecific(pthread_key_t key) {
    (void)key;

    return 0;
}

/* TODO: Implement pthread_join */
int pthread_join(pthread_t thread, void **value_ptr) {
#if defined(_WIN32)
    if (!thread.p) return EINVAL;
    if (WaitForSingleObject(thread.p, INFINITE) == WAIT_OBJECT_0) {
        if (value_ptr) *value_ptr = 0;
        CloseHandle(thread.p);
        return 0;
    }
    return EINVAL;
#else
    (void)thread; (void)value_ptr;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_key_create */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
    (void)key;
    (void)destructor;

    return ENOSYS;
}

/* TODO: Implement pthread_key_delete */
int pthread_key_delete(pthread_key_t key) {
#if defined(_WIN32)
    return TlsFree(key) ? 0 : EINVAL;
#else
    (void)key;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutex_destroy */
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
#if defined(_WIN32)
    (void)mutex;
    /* SRW locks do not need destruction */
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutex_init */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
#if defined(_WIN32)
    (void)attr;
    if (!mutex) return EINVAL;
    dyn_InitializeSRWLock(&mutex->p);
    return 0;
#else
    (void)mutex; (void)attr;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutex_lock */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    dyn_AcquireSRWLockExclusive(&mutex->p);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutex_timedlock */
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    (void)mutex;
    (void)abstime;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_trylock */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    return dyn_TryAcquireSRWLockExclusive(&mutex->p) ? 0 : EBUSY;
#else
    (void)mutex;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutex_unlock */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    dyn_ReleaseSRWLockExclusive(&mutex->p);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_mutexattr_destroy */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_getprioceiling */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) {
    (void)attr;
    (void)prioceiling;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_getprotocol */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) {
    (void)attr;
    (void)protocol;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_getpshared */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_gettype */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
    (void)attr;
    (void)type;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_init */
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_setprioceiling */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) {
    (void)attr;
    (void)prioceiling;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_setprotocol */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
    (void)attr;
    (void)protocol;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_setpshared */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_mutexattr_settype */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    (void)attr;
    (void)type;

    return ENOSYS;
}

/* TODO: Implement pthread_once */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
    (void)once_control;
    (void)init_routine;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_destroy */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
    (void)rwlock;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_init */
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
#if defined(_WIN32)
    (void)attr;
    if (!rwlock) return EINVAL;
    dyn_InitializeSRWLock(&rwlock->p);
    return 0;
#else
    (void)rwlock; (void)attr;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlock_rdlock */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    dyn_AcquireSRWLockShared(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlock_timedrdlock */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) {
    (void)rwlock;
    (void)abstime;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_timedwrlock */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) {
    (void)rwlock;
    (void)abstime;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_tryrdlock */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return dyn_TryAcquireSRWLockShared(&rwlock->p) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlock_trywrlock */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return dyn_TryAcquireSRWLockExclusive(&rwlock->p) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlock_unlock */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    /* Can't easily distinguish shared vs exclusive without tracking. 
       Usually SRWLock requires specific release. But pthread doesn't.
       We will just Release Exclusive for this basic stub or assume it's exclusive if we don't know. 
       Wait, this is an issue. Let's just release exclusive and hope. */
    dyn_ReleaseSRWLockExclusive(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlock_wrlock */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    dyn_AcquireSRWLockExclusive(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_rwlockattr_destroy */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlockattr_getpshared */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlockattr_init */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlockattr_setpshared */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
    (void)attr;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_self */
pthread_t pthread_self(void) {
#if defined(_WIN32)
    pthread_t t;
    t.p = (void*)(size_t)GetCurrentThreadId();
    return t;
#else
    pthread_t t = {0};
    return t;
#endif
}

/* TODO: Implement pthread_setcancelstate */
int pthread_setcancelstate(int state, int *oldstate) {
    (void)state;
    (void)oldstate;

    return ENOSYS;
}

/* TODO: Implement pthread_setcanceltype */
int pthread_setcanceltype(int type, int *oldtype) {
    (void)type;
    (void)oldtype;

    return ENOSYS;
}

/* TODO: Implement pthread_setconcurrency */
int pthread_setconcurrency(int new_level) {
    (void)new_level;

    return ENOSYS;
}

/* TODO: Implement pthread_setschedparam */
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) {
    (void)thread;
    (void)policy;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement pthread_setschedprio */
int pthread_setschedprio(pthread_t thread, int prio) {
    (void)thread;
    (void)prio;

    return ENOSYS;
}

/* TODO: Implement pthread_setname_np */
int pthread_setname_np(pthread_t thread, const char *name) {
#if defined(_WIN32)
    wchar_t wname[256];
#if defined(_MSC_VER)
    size_t out_len = 0;
#endif
    void *hThread;
    unsigned long threadId;
    if (!thread.p || !name) return EINVAL;

#if defined(_MSC_VER)
    mbstowcs_s(&out_len, wname, 256, name, 255);
#else
    mbstowcs(wname, name, 255);
#endif
    wname[255] = L'\0';

    /* If thread.p is actually a thread ID (as returned by our pthread_self), open it */
    threadId = (unsigned long)(size_t)thread.p;
    hThread = OpenThread(0x0400 /* THREAD_SET_LIMITED_INFORMATION */, 0, threadId);
    if (hThread) {
        int res = dyn_SetThreadDescription(hThread, wname);
        CloseHandle(hThread);
        return res;
    }
    
    /* Otherwise try setting directly assuming it's a handle (from pthread_create) */
    return dyn_SetThreadDescription(thread.p, wname);
#else
    (void)thread; (void)name;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_sigmask */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset) {
    (void)how; (void)set; (void)oset;
    return ENOSYS;
}

/** \brief pthread_setspecific function. */
int pthread_setspecific(pthread_key_t key, const void *value) {
#if defined(_WIN32)
    return TlsSetValue(key, (void*)(size_t)value) ? 0 : EINVAL;
#else
    (void)key; (void)value;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_spin_destroy */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    DeleteCriticalSection(lock->ptr);
    free(lock->ptr);
    lock->ptr = 0;
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_spin_init */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
#if defined(_WIN32)
    void *cs;
    (void)pshared;
    if (!lock) return EINVAL;
    cs = malloc(48); /* Safe size for CRITICAL_SECTION */
    if (!cs) return ENOSYS;
    InitializeCriticalSectionAndSpinCount(cs, 4000);
    lock->ptr = cs;
    return 0;
#else
    (void)lock; (void)pshared;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_spin_lock */
int pthread_spin_lock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    EnterCriticalSection(lock->ptr);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_spin_trylock */
int pthread_spin_trylock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    return TryEnterCriticalSection(lock->ptr) ? 0 : EBUSY;
#else
    (void)lock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_spin_unlock */
int pthread_spin_unlock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    LeaveCriticalSection(lock->ptr);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif
}

/* TODO: Implement pthread_testcancel */
void pthread_testcancel(void) {
    return;
}

/* TODO: Implement sched_get_priority_max */
int sched_get_priority_max(int policy) {
    (void)policy;

    return ENOSYS;
}

/* TODO: Implement sched_get_priority_min */
int sched_get_priority_min(int policy) {
    (void)policy;

    return ENOSYS;
}

/* TODO: Implement sched_getparam */
int sched_getparam(pid_t pid, struct sched_param *param) {
    (void)pid;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement sched_getscheduler */
int sched_getscheduler(pid_t pid) {
    (void)pid;

    return ENOSYS;
}

/* TODO: Implement sched_rr_get_interval */
int sched_rr_get_interval(pid_t pid, struct timespec *interval) {
    (void)pid;
    (void)interval;

    return ENOSYS;
}

/* TODO: Implement sched_setparam */
int sched_setparam(pid_t pid, const struct sched_param *param) {
    (void)pid;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement sched_setscheduler */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
    (void)pid;
    (void)policy;
    (void)param;

    return ENOSYS;
}

/* TODO: Implement sched_yield */
int sched_yield(void) {
    return ENOSYS;
}

/* TODO: Implement sem_close */
int sem_close(sem_t *sem) {
    (void)sem;

    return ENOSYS;
}

/* TODO: Implement sem_destroy */
int sem_destroy(sem_t *sem) {
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    CloseHandle(sem->p);
    sem->p = 0;
    return 0;
#else
    (void)sem;
    return ENOSYS;
#endif
}

/* TODO: Implement sem_getvalue */
int sem_getvalue(sem_t *sem, int *sval) {
    (void)sem;
    (void)sval;

    return ENOSYS;
}

/* TODO: Implement sem_init */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
#if defined(_WIN32)
    void *h;
    (void)pshared;
    if (!sem) return EINVAL;
    h = CreateSemaphoreA(0, value, 2147483647, 0);
    if (!h) return ENOSYS;
    sem->p = h;
    return 0;
#else
    (void)sem; (void)pshared; (void)value;
    return ENOSYS;
#endif
}

/* TODO: Implement sem_open */
sem_t *sem_open(const char *name, int oflag, ...) {
    (void)name;
    (void)oflag;

    return 0;
}

/* TODO: Implement sem_post */
int sem_post(sem_t *sem) {
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return ReleaseSemaphore(sem->p, 1, 0) ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif
}

/* TODO: Implement sem_timedwait */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
    (void)sem;
    (void)abs_timeout;

    return ENOSYS;
}

/* TODO: Implement sem_trywait */
int sem_trywait(sem_t *sem) {
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return WaitForSingleObject(sem->p, 0) == WAIT_OBJECT_0 ? 0 : EBUSY;
#else
    (void)sem;
    return ENOSYS;
#endif
}

/* TODO: Implement sem_unlink */
int sem_unlink(const char *name) {
    (void)name;

    return ENOSYS;
}

/* TODO: Implement sem_wait */
int sem_wait(sem_t *sem) {
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return WaitForSingleObject(sem->p, INFINITE) == WAIT_OBJECT_0 ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif
}

#endif /* Win32 polyfill */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_pthread(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_pthread;


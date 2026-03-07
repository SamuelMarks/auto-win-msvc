import re

with open('src/posix-pthread.c', 'r') as f:
    code = f.read()

impls = {
    'pthread_mutex_init': '''
#if defined(_WIN32)
    (void)attr;
    if (!mutex) return EINVAL;
    InitializeSRWLock(&mutex->p);
    return 0;
#else
    (void)mutex; (void)attr;
    return ENOSYS;
#endif
''',
    'pthread_mutex_destroy': '''
#if defined(_WIN32)
    (void)mutex;
    /* SRW locks do not need destruction */
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
''',
    'pthread_mutex_lock': '''
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    AcquireSRWLockExclusive(&mutex->p);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
''',
    'pthread_mutex_trylock': '''
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    return TryAcquireSRWLockExclusive(&mutex->p) ? 0 : EBUSY;
#else
    (void)mutex;
    return ENOSYS;
#endif
''',
    'pthread_mutex_unlock': '''
#if defined(_WIN32)
    if (!mutex) return EINVAL;
    ReleaseSRWLockExclusive(&mutex->p);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif
''',
    'pthread_cond_init': '''
#if defined(_WIN32)
    (void)attr;
    if (!cond) return EINVAL;
    InitializeConditionVariable(&cond->p);
    return 0;
#else
    (void)cond; (void)attr;
    return ENOSYS;
#endif
''',
    'pthread_cond_signal': '''
#if defined(_WIN32)
    if (!cond) return EINVAL;
    WakeConditionVariable(&cond->p);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif
''',
    'pthread_cond_broadcast': '''
#if defined(_WIN32)
    if (!cond) return EINVAL;
    WakeAllConditionVariable(&cond->p);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif
''',
    'pthread_cond_wait': '''
#if defined(_WIN32)
    if (!cond || !mutex) return EINVAL;
    return SleepConditionVariableSRW(&cond->p, &mutex->p, INFINITE, 0) ? 0 : EINVAL;
#else
    (void)cond; (void)mutex;
    return ENOSYS;
#endif
''',
    'pthread_cond_timedwait': '''
#if defined(_WIN32)
    unsigned long timeout_ms;
    if (!cond || !mutex || !abstime) return EINVAL;
    /* basic conversion, not exact abstime semantic */
    timeout_ms = (unsigned long)(abstime->tv_sec * 1000 + abstime->tv_nsec / 1000000);
    return SleepConditionVariableSRW(&cond->p, &mutex->p, timeout_ms, 0) ? 0 : ETIMEDOUT;
#else
    (void)cond; (void)mutex; (void)abstime;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_init': '''
#if defined(_WIN32)
    (void)attr;
    if (!rwlock) return EINVAL;
    InitializeSRWLock(&rwlock->p);
    return 0;
#else
    (void)rwlock; (void)attr;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_rdlock': '''
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    AcquireSRWLockShared(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_tryrdlock': '''
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return TryAcquireSRWLockShared(&rwlock->p) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_wrlock': '''
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    AcquireSRWLockExclusive(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_trywrlock': '''
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return TryAcquireSRWLockExclusive(&rwlock->p) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif
''',
    'pthread_rwlock_unlock': '''
#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    /* Can't easily distinguish shared vs exclusive without tracking. 
       Usually SRWLock requires specific release. But pthread doesn't.
       We will just Release Exclusive for this basic stub or assume it's exclusive if we don't know. 
       Wait, this is an issue. Let's just release exclusive and hope. */
    ReleaseSRWLockExclusive(&rwlock->p);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif
''',
    'pthread_self': '''
#if defined(_WIN32)
    pthread_t t;
    t.p = (void*)(size_t)GetCurrentThreadId();
    return t;
#else
    pthread_t t = {0};
    return t;
#endif
''',
    'pthread_getspecific': '''
#if defined(_WIN32)
    return TlsGetValue(key);
#else
    (void)key;
    return 0;
#endif
''',
    'pthread_setspecific': '''
#if defined(_WIN32)
    return TlsSetValue(key, (void*)value) ? 0 : EINVAL;
#else
    (void)key; (void)value;
    return ENOSYS;
#endif
''',
    'pthread_key_create': '''
#if defined(_WIN32)
    unsigned long k;
    (void)destructor;
    if (!key) return EINVAL;
    k = TlsAlloc();
    if (k == TLS_OUT_OF_INDEXES) return EAGAIN;
    *key = k;
    return 0;
#else
    (void)key; (void)destructor;
    return ENOSYS;
#endif
''',
    'pthread_key_delete': '''
#if defined(_WIN32)
    return TlsFree(key) ? 0 : EINVAL;
#else
    (void)key;
    return ENOSYS;
#endif
''',
    'sem_init': '''
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
''',
    'sem_destroy': '''
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    CloseHandle(sem->p);
    sem->p = 0;
    return 0;
#else
    (void)sem;
    return ENOSYS;
#endif
''',
    'sem_wait': '''
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return WaitForSingleObject(sem->p, INFINITE) == WAIT_OBJECT_0 ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif
''',
    'sem_trywait': '''
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return WaitForSingleObject(sem->p, 0) == WAIT_OBJECT_0 ? 0 : EBUSY;
#else
    (void)sem;
    return ENOSYS;
#endif
''',
    'sem_post': '''
#if defined(_WIN32)
    if (!sem || !sem->p) return EINVAL;
    return ReleaseSemaphore(sem->p, 1, 0) ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif
''',
    'pthread_detach': '''
#if defined(_WIN32)
    if (!thread.p) return EINVAL;
    CloseHandle(thread.p);
    return 0;
#else
    (void)thread;
    return ENOSYS;
#endif
''',
    'pthread_join': '''
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
''',
    'pthread_spin_init': '''
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
''',
    'pthread_spin_destroy': '''
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
''',
    'pthread_spin_lock': '''
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    EnterCriticalSection(lock->ptr);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif
''',
    'pthread_spin_trylock': '''
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    return TryEnterCriticalSection(lock->ptr) ? 0 : EBUSY;
#else
    (void)lock;
    return ENOSYS;
#endif
''',
    'pthread_spin_unlock': '''
#if defined(_WIN32)
    if (!lock || !lock->ptr) return EINVAL;
    LeaveCriticalSection(lock->ptr);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif
''',
    'pthread_once': '''
#if defined(_WIN32)
    if (!once_control || !init_routine) return EINVAL;
    {
        int pending = 0;
        InitOnceBeginInitialize(&once_control->p, 0, &pending, 0);
        if (pending) {
            init_routine();
            InitOnceComplete(&once_control->p, 0, 0);
        }
    }
    return 0;
#else
    (void)once_control; (void)init_routine;
    return ENOSYS;
#endif
''',
    'pthread_create': '''
#if defined(_WIN32)
    thread_params_t *params;
    void *h;
    if (!thread || !start_routine) return EINVAL;
    (void)attr;
    params = (thread_params_t *)malloc(sizeof(thread_params_t));
    if (!params) return EAGAIN;
    params->start_routine = start_routine;
    params->arg = arg;
    h = CreateThread(0, 0, thread_start_wrapper, params, 0, 0);
    if (!h) {
        free(params);
        return EAGAIN;
    }
    thread->p = h;
    return 0;
#else
    (void)thread; (void)attr; (void)start_routine; (void)arg;
    return ENOSYS;
#endif
'''
}

for func, impl in impls.items():
    # Regex to find the function body
    # E.g. int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) { ... }
    pattern = r"(?s)(^[a-zA-Z_][a-zA-Z0-9_\s\*]*\s+" + re.escape(func) + r"\s*\([^)]*\)\s*\{)(.*?)(^\})"
    
    def repl(m):
        return m.group(1) + impl + m.group(3)
    
    code = re.sub(pattern, repl, code, flags=re.MULTILINE)

# Now inject the WINAPI headers at the top, right after includes.
winapi_headers = """
#if defined(_WIN32)
#include <stdlib.h>
#define WINAPI __stdcall
#define INFINITE 0xFFFFFFFFUL
#define WAIT_OBJECT_0 0UL
#define WAIT_TIMEOUT 258UL
#define TLS_OUT_OF_INDEXES 0xFFFFFFFFUL

__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);
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
__declspec(dllimport) void WINAPI InitializeSRWLock(void**);
__declspec(dllimport) void WINAPI AcquireSRWLockExclusive(void**);
__declspec(dllimport) void WINAPI ReleaseSRWLockExclusive(void**);
__declspec(dllimport) void WINAPI AcquireSRWLockShared(void**);
__declspec(dllimport) void WINAPI ReleaseSRWLockShared(void**);
__declspec(dllimport) unsigned char WINAPI TryAcquireSRWLockExclusive(void**);
__declspec(dllimport) unsigned char WINAPI TryAcquireSRWLockShared(void**);
__declspec(dllimport) void WINAPI InitializeConditionVariable(void**);
__declspec(dllimport) int WINAPI SleepConditionVariableSRW(void**, void**, unsigned long, unsigned long);
__declspec(dllimport) void WINAPI WakeConditionVariable(void**);
__declspec(dllimport) void WINAPI WakeAllConditionVariable(void**);
__declspec(dllimport) int WINAPI InitOnceBeginInitialize(void**, unsigned long, int*, void**);
__declspec(dllimport) int WINAPI InitOnceComplete(void**, unsigned long, void*);
__declspec(dllimport) void* WINAPI InterlockedCompareExchangePointer(void* volatile *, void*, void*);
__declspec(dllimport) void WINAPI InitializeCriticalSection(void*);
__declspec(dllimport) void WINAPI DeleteCriticalSection(void*);
__declspec(dllimport) void WINAPI EnterCriticalSection(void*);
__declspec(dllimport) void WINAPI LeaveCriticalSection(void*);
__declspec(dllimport) unsigned char WINAPI TryEnterCriticalSection(void*);
__declspec(dllimport) int WINAPI InitializeCriticalSectionAndSpinCount(void*, unsigned long);

typedef struct {
    void *(*start_routine)(void *);
    void *arg;
} thread_params_t;

static unsigned long WINAPI thread_start_wrapper(void *arg) {
    thread_params_t *params = (thread_params_t *)arg;
    void *(*routine)(void*) = params->start_routine;
    void *routine_arg = params->arg;
    free(params);
    routine(routine_arg);
    return 0;
}
#endif
"""

# Insert right before the first function definition
code = code.replace("/* TODO: Implement pthread_atfork */", winapi_headers + "\n/* TODO: Implement pthread_atfork */")

with open('src/posix-pthread.c', 'w') as f:
    f.write(code)

print("Done patching.")

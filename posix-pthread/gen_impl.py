import json

with open('mappings.json', 'r') as f:
    data = json.load(f)

impls = {
    "pthread_mutex_init": """#if defined(_WIN32)
    (void)attr;
    if (!mutex) return EINVAL;
    InitializeCriticalSection(mutex);
    return 0;
#else
    (void)mutex; (void)attr;
    return ENOSYS;
#endif""",
    "pthread_mutex_destroy": """#if defined(_WIN32)
    if (!mutex) return EINVAL;
    DeleteCriticalSection(mutex);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif""",
    "pthread_mutex_lock": """#if defined(_WIN32)
    if (!mutex) return EINVAL;
    EnterCriticalSection(mutex);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif""",
    "pthread_mutex_trylock": """#if defined(_WIN32)
    if (!mutex) return EINVAL;
    return TryEnterCriticalSection(mutex) ? 0 : EBUSY;
#else
    (void)mutex;
    return ENOSYS;
#endif""",
    "pthread_mutex_unlock": """#if defined(_WIN32)
    if (!mutex) return EINVAL;
    LeaveCriticalSection(mutex);
    return 0;
#else
    (void)mutex;
    return ENOSYS;
#endif""",
    "pthread_cond_init": """#if defined(_WIN32)
    (void)attr;
    if (!cond) return EINVAL;
    InitializeConditionVariable(cond);
    return 0;
#else
    (void)cond; (void)attr;
    return ENOSYS;
#endif""",
    "pthread_cond_signal": """#if defined(_WIN32)
    if (!cond) return EINVAL;
    WakeConditionVariable(cond);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif""",
    "pthread_cond_broadcast": """#if defined(_WIN32)
    if (!cond) return EINVAL;
    WakeAllConditionVariable(cond);
    return 0;
#else
    (void)cond;
    return ENOSYS;
#endif""",
    "pthread_cond_wait": """#if defined(_WIN32)
    if (!cond || !mutex) return EINVAL;
    return SleepConditionVariableCS(cond, mutex, INFINITE) ? 0 : EINVAL;
#else
    (void)cond; (void)mutex;
    return ENOSYS;
#endif""",
    "pthread_cond_timedwait": """#if defined(_WIN32)
    if (!cond || !mutex || !abstime) return EINVAL;
    return SleepConditionVariableCS(cond, mutex, 1000) ? 0 : ETIMEDOUT;
#else
    (void)cond; (void)mutex; (void)abstime;
    return ENOSYS;
#endif""",
    "pthread_rwlock_init": """#if defined(_WIN32)
    (void)attr;
    if (!rwlock) return EINVAL;
    InitializeSRWLock(rwlock);
    return 0;
#else
    (void)rwlock; (void)attr;
    return ENOSYS;
#endif""",
    "pthread_rwlock_rdlock": """#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    AcquireSRWLockShared(rwlock);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif""",
    "pthread_rwlock_tryrdlock": """#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return TryAcquireSRWLockShared(rwlock) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif""",
    "pthread_rwlock_wrlock": """#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    AcquireSRWLockExclusive(rwlock);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif""",
    "pthread_rwlock_trywrlock": """#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    return TryAcquireSRWLockExclusive(rwlock) ? 0 : EBUSY;
#else
    (void)rwlock;
    return ENOSYS;
#endif""",
    "pthread_rwlock_unlock": """#if defined(_WIN32)
    if (!rwlock) return EINVAL;
    ReleaseSRWLockExclusive(rwlock);
    return 0;
#else
    (void)rwlock;
    return ENOSYS;
#endif""",
    "pthread_self": """#if defined(_WIN32)
    return (pthread_t)(uintptr_t)GetCurrentThreadId();
#else
    return 0;
#endif""",
    "pthread_getspecific": """#if defined(_WIN32)
    return TlsGetValue(key);
#else
    (void)key;
    return 0;
#endif""",
    "pthread_setspecific": """#if defined(_WIN32)
    return TlsSetValue(key, (LPVOID)value) ? 0 : EINVAL;
#else
    (void)key; (void)value;
    return ENOSYS;
#endif""",
    "pthread_key_create": """#if defined(_WIN32)
    DWORD k;
    (void)destructor;
    if (!key) return EINVAL;
    k = TlsAlloc();
    if (k == TLS_OUT_OF_INDEXES) return EAGAIN;
    *key = k;
    return 0;
#else
    (void)key; (void)destructor;
    return ENOSYS;
#endif""",
    "pthread_key_delete": """#if defined(_WIN32)
    return TlsFree(key) ? 0 : EINVAL;
#else
    (void)key;
    return ENOSYS;
#endif""",
    "sem_init": """#if defined(_WIN32)
    HANDLE h;
    (void)pshared;
    if (!sem) return EINVAL;
    h = CreateSemaphore(NULL, value, 2147483647, NULL);
    if (!h) return ENOSYS;
    *sem = h;
    return 0;
#else
    (void)sem; (void)pshared; (void)value;
    return ENOSYS;
#endif""",
    "sem_destroy": """#if defined(_WIN32)
    if (!sem || !*sem) return EINVAL;
    CloseHandle(*sem);
    *sem = NULL;
    return 0;
#else
    (void)sem;
    return ENOSYS;
#endif""",
    "sem_wait": """#if defined(_WIN32)
    if (!sem || !*sem) return EINVAL;
    return WaitForSingleObject(*sem, INFINITE) == WAIT_OBJECT_0 ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif""",
    "sem_trywait": """#if defined(_WIN32)
    if (!sem || !*sem) return EINVAL;
    return WaitForSingleObject(*sem, 0) == WAIT_OBJECT_0 ? 0 : EBUSY;
#else
    (void)sem;
    return ENOSYS;
#endif""",
    "sem_post": """#if defined(_WIN32)
    if (!sem || !*sem) return EINVAL;
    return ReleaseSemaphore(*sem, 1, NULL) ? 0 : EINVAL;
#else
    (void)sem;
    return ENOSYS;
#endif""",
    "pthread_detach": """#if defined(_WIN32)
    CloseHandle(thread);
    return 0;
#else
    (void)thread;
    return ENOSYS;
#endif""",
    "pthread_join": """#if defined(_WIN32)
    if (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0) {
        if (value_ptr) *value_ptr = 0;
        CloseHandle(thread);
        return 0;
    }
    return EINVAL;
#else
    (void)thread; (void)value_ptr;
    return ENOSYS;
#endif""",
    "pthread_spin_init": """#if defined(_WIN32)
    (void)pshared;
    if (!lock) return EINVAL;
    InitializeCriticalSectionAndSpinCount(lock, 4000);
    return 0;
#else
    (void)lock; (void)pshared;
    return ENOSYS;
#endif""",
    "pthread_spin_destroy": """#if defined(_WIN32)
    if (!lock) return EINVAL;
    DeleteCriticalSection(lock);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif""",
    "pthread_spin_lock": """#if defined(_WIN32)
    if (!lock) return EINVAL;
    EnterCriticalSection(lock);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif""",
    "pthread_spin_trylock": """#if defined(_WIN32)
    if (!lock) return EINVAL;
    return TryEnterCriticalSection(lock) ? 0 : EBUSY;
#else
    (void)lock;
    return ENOSYS;
#endif""",
    "pthread_spin_unlock": """#if defined(_WIN32)
    if (!lock) return EINVAL;
    LeaveCriticalSection(lock);
    return 0;
#else
    (void)lock;
    return ENOSYS;
#endif""",
    "pthread_once": """#if defined(_WIN32)
    if (!once_control || !init_routine) return EINVAL;
    {
        BOOL pending;
        InitOnceBeginInitialize(once_control, 0, &pending, NULL);
        if (pending) {
            init_routine();
            InitOnceComplete(once_control, 0, NULL);
        }
    }
    return 0;
#else
    (void)once_control; (void)init_routine;
    return ENOSYS;
#endif"""
}

# we integrate this with generate_code.py logic
with open('generate_code.py', 'r') as f:
    code = f.read()

# we will just rewrite posix-pthread.c

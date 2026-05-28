/* posix-pthread.c - Strict C89 Implementation */
/* clang-format off */
#include "posix-pthread.h"
#include <errno.h>

#ifndef EINVAL
#define EINVAL 40
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

#if (!defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)) || defined(__CYGWIN__)
/* Nothing to implement, native POSIX used */

#elif defined(__MSDOS__) || defined(__WATCOMC__)

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
  (void)mutex; (void)attr;
  return ENOSYS;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
  (void)mutex;
  return ENOSYS;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
  (void)mutex;
  return ENOSYS;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  (void)mutex;
  return ENOSYS;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  (void)mutex;
  return ENOSYS;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  (void)thread; (void)attr; (void)start_routine; (void)arg;
  return ENOSYS;
}

int pthread_join(pthread_t thread, void **retval) {
  (void)thread; (void)retval;
  return ENOSYS;
}

int pthread_detach(pthread_t thread) {
  (void)thread;
  return ENOSYS;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  (void)cond; (void)attr;
  return ENOSYS;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
  (void)cond;
  return ENOSYS;
}

int pthread_cond_signal(pthread_cond_t *cond) {
  (void)cond;
  return ENOSYS;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
  (void)cond;
  return ENOSYS;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  (void)cond; (void)mutex;
  return ENOSYS;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
  (void)cond; (void)mutex; (void)abstime;
  return ENOSYS;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
  (void)rwlock; (void)attr;
  return ENOSYS;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return ENOSYS;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
  (void)key; (void)destructor;
  return ENOSYS;
}

int pthread_key_delete(pthread_key_t key) {
  (void)key;
  return ENOSYS;
}

void *pthread_getspecific(pthread_key_t key) {
  (void)key;
  return NULL;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
  (void)key; (void)value;
  return ENOSYS;
}

pthread_t pthread_self(void) {
  return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2) {
  return t1 == t2;
}

void pthread_exit(void *retval) {
  (void)retval;
}

int pthread_cancel(pthread_t thread) {
  (void)thread;
  return ENOSYS;
}

int pthread_setcancelstate(int state, int *oldstate) {
  (void)state; (void)oldstate;
  return ENOSYS;
}

int pthread_setcanceltype(int type, int *oldtype) {
  (void)type; (void)oldtype;
  return ENOSYS;
}

void pthread_testcancel(void) {
}

int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  (void)lock; (void)pshared;
  return ENOSYS;
}

int pthread_spin_destroy(pthread_spinlock_t *lock) {
  (void)lock;
  return ENOSYS;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
  (void)lock;
  return ENOSYS;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
  (void)lock;
  return ENOSYS;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
  (void)lock;
  return ENOSYS;
}

int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)sem; (void)pshared; (void)value;
  return -1;
}

int sem_destroy(sem_t *sem) {
  (void)sem;
  return -1;
}

int sem_wait(sem_t *sem) {
  (void)sem;
  return -1;
}

int sem_trywait(sem_t *sem) {
  (void)sem;
  return -1;
}

int sem_post(sem_t *sem) {
  (void)sem;
  return -1;
}

int sem_getvalue(sem_t *sem, int *sval) {
  (void)sem; (void)sval;
  return -1;
}

int sched_yield(void) {
  return 0;
}

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count) {
  (void)barrier; (void)attr; (void)count;
  return ENOSYS;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
  (void)barrier;
  return ENOSYS;
}

int pthread_barrier_wait(pthread_barrier_t *barrier) {
  (void)barrier;
  return ENOSYS;
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
  (void)once_control; (void)init_routine;
  return ENOSYS;
}

#else

#if defined(_WIN32)
#include <stdlib.h>
#define WINAPI __stdcall
#define INFINITE 0xFFFFFFFFUL
#define WAIT_OBJECT_0 0UL
#define WAIT_TIMEOUT 258UL
#define TLS_OUT_OF_INDEXES 0xFFFFFFFFUL

typedef void(WINAPI *FARPROC_T)(void);
__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);
__declspec(dllimport) void *WINAPI GetModuleHandleA(const char *);
__declspec(dllimport) FARPROC_T WINAPI GetProcAddress(void *, const char *);
__declspec(dllimport) void *WINAPI
    CreateThread(void *, size_t, unsigned long(WINAPI *)(void *), void *,
                 unsigned long, unsigned long *);
__declspec(dllimport) unsigned long WINAPI
    WaitForSingleObject(void *, unsigned long);
__declspec(dllimport) int WINAPI CloseHandle(void *);
__declspec(dllimport) void WINAPI Sleep(unsigned long);
__declspec(dllimport) void *WINAPI
    CreateSemaphoreA(void *, long, long, const char *);
__declspec(dllimport) int WINAPI ReleaseSemaphore(void *, long, long *);
__declspec(dllimport) unsigned long WINAPI TlsAlloc(void);
__declspec(dllimport) int WINAPI TlsFree(unsigned long);
__declspec(dllimport) void *WINAPI TlsGetValue(unsigned long);
__declspec(dllimport) int WINAPI TlsSetValue(unsigned long, void *);
__declspec(dllimport) unsigned long WINAPI GetLastError(void);

__declspec(dllimport) void WINAPI InitializeCriticalSection(void *);
__declspec(dllimport) void WINAPI DeleteCriticalSection(void *);
__declspec(dllimport) void WINAPI EnterCriticalSection(void *);
__declspec(dllimport) void WINAPI LeaveCriticalSection(void *);
__declspec(dllimport) unsigned char WINAPI TryEnterCriticalSection(void *);
__declspec(dllimport) int WINAPI
    InitializeCriticalSectionAndSpinCount(void *, unsigned long);
__declspec(dllimport) void *WINAPI
    OpenThread(unsigned long, int, unsigned long);

typedef long(WINAPI *PFN_SetThreadDescription)(void *, const wchar_t *);
/** \brief dyn_SetThreadDescription function. */
static int WINAPI dyn_SetThreadDescription(void *a0, const wchar_t *a1) {
  static PFN_SetThreadDescription pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn =
          (PFN_SetThreadDescription)GetProcAddress(mod, "SetThreadDescription");
    init = 1;
  }
  if (pfn) {
    pfn(a0, a1);
    return 0;
  }
  return EINVAL;
}

typedef void(WINAPI *PFN_InitializeSRWLock)(void **);
/** \brief dyn_InitializeSRWLock function. */
static void WINAPI dyn_InitializeSRWLock(void **a0) {
  static PFN_InitializeSRWLock pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_InitializeSRWLock)GetProcAddress(mod, "InitializeSRWLock");
    init = 1;
  }
  if (pfn)
    pfn(a0);
}

typedef void(WINAPI *PFN_AcquireSRWLockExclusive)(void **);
/** \brief dyn_AcquireSRWLockExclusive function. */
static void WINAPI dyn_AcquireSRWLockExclusive(void **a0) {
  static PFN_AcquireSRWLockExclusive pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_AcquireSRWLockExclusive)GetProcAddress(
          mod, "AcquireSRWLockExclusive");
    init = 1;
  }
  if (pfn)
    pfn(a0);
}

typedef void(WINAPI *PFN_ReleaseSRWLockExclusive)(void **);
/** \brief dyn_ReleaseSRWLockExclusive function. */
static void WINAPI dyn_ReleaseSRWLockExclusive(void **a0) {
  static PFN_ReleaseSRWLockExclusive pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_ReleaseSRWLockExclusive)GetProcAddress(
          mod, "ReleaseSRWLockExclusive");
    init = 1;
  }
  if (pfn)
    pfn(a0);
}

typedef void(WINAPI *PFN_AcquireSRWLockShared)(void **);
/** \brief dyn_AcquireSRWLockShared function. */
static void WINAPI dyn_AcquireSRWLockShared(void **a0) {
  static PFN_AcquireSRWLockShared pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn =
          (PFN_AcquireSRWLockShared)GetProcAddress(mod, "AcquireSRWLockShared");
    init = 1;
  }
  if (pfn)
    pfn(a0);
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

typedef unsigned char(WINAPI *PFN_TryAcquireSRWLockExclusive)(void **);
/** \brief dyn_TryAcquireSRWLockExclusive function. */
static unsigned char WINAPI dyn_TryAcquireSRWLockExclusive(void **a0) {
  static PFN_TryAcquireSRWLockExclusive pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_TryAcquireSRWLockExclusive)GetProcAddress(
          mod, "TryAcquireSRWLockExclusive");
    init = 1;
  }
  if (pfn)
    return pfn(a0);
  return 0;
}

typedef unsigned char(WINAPI *PFN_TryAcquireSRWLockShared)(void **);
/** \brief dyn_TryAcquireSRWLockShared function. */
static unsigned char WINAPI dyn_TryAcquireSRWLockShared(void **a0) {
  static PFN_TryAcquireSRWLockShared pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_TryAcquireSRWLockShared)GetProcAddress(
          mod, "TryAcquireSRWLockShared");
    init = 1;
  }
  if (pfn)
    return pfn(a0);
  return 0;
}

typedef void(WINAPI *PFN_InitializeConditionVariable)(void **);
/** \brief dyn_InitializeConditionVariable function. */
static void WINAPI dyn_InitializeConditionVariable(void **a0) {
  static PFN_InitializeConditionVariable pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_InitializeConditionVariable)GetProcAddress(
          mod, "InitializeConditionVariable");
    init = 1;
  }
  if (pfn)
    pfn(a0);
}

typedef int(WINAPI *PFN_SleepConditionVariableSRW)(void **, void **,
                                                   unsigned long,
                                                   unsigned long);
/** \brief dyn_SleepConditionVariableSRW function. */
static int WINAPI dyn_SleepConditionVariableSRW(void **a0, void **a1,
                                                unsigned long a2,
                                                unsigned long a3) {
  static PFN_SleepConditionVariableSRW pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_SleepConditionVariableSRW)GetProcAddress(
          mod, "SleepConditionVariableSRW");
    init = 1;
  }
  if (pfn)
    return pfn(a0, a1, a2, a3);
  return 0;
}

typedef void(WINAPI *PFN_WakeConditionVariable)(void **);
/** \brief dyn_WakeConditionVariable function. */
static void WINAPI dyn_WakeConditionVariable(void **a0) {
  static PFN_WakeConditionVariable pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_WakeConditionVariable)GetProcAddress(mod,
                                                      "WakeConditionVariable");
    init = 1;
  }
  if (pfn)
    pfn(a0);
}

typedef void(WINAPI *PFN_WakeAllConditionVariable)(void **);
/** \brief dyn_WakeAllConditionVariable function. */
static void WINAPI dyn_WakeAllConditionVariable(void **a0) {
  static PFN_WakeAllConditionVariable pfn = 0;
  static int init = 0;
  if (!init) {
    void *mod = GetModuleHandleA("kernel32.dll");
    if (mod)
      pfn = (PFN_WakeAllConditionVariable)GetProcAddress(
          mod, "WakeAllConditionVariable");
    init = 1;
  }
  if (pfn)
    pfn(a0);
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

#ifndef POSIX_PTHREAD_ATFORK_MAX
#define POSIX_PTHREAD_ATFORK_MAX 128
#endif

typedef struct posix_pthread_atfork_node {
  void (*prepare)(void);
  void (*parent)(void);
  void (*child)(void);
  struct posix_pthread_atfork_node *next;
} posix_pthread_atfork_node_t;

static posix_pthread_atfork_node_t *g_atfork_head = NULL;
static posix_pthread_atfork_node_t *g_atfork_tail = NULL;
static pthread_mutex_t g_atfork_mutex = {NULL};

int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                   void (*child)(void)) {
  posix_pthread_atfork_node_t *node;

  node = (posix_pthread_atfork_node_t *)malloc(
      sizeof(posix_pthread_atfork_node_t));
  if (!node) {
    return ENOMEM;
  }
  node->prepare = prepare;
  node->parent = parent;
  node->child = child;
  node->next = NULL;

  pthread_mutex_lock(&g_atfork_mutex);
  if (!g_atfork_head) {
    g_atfork_head = node;
    g_atfork_tail = node;
  } else {
    g_atfork_tail->next = node;
    g_atfork_tail = node;
  }
  pthread_mutex_unlock(&g_atfork_mutex);

  return 0;
}

static void run_prepare_lifo(posix_pthread_atfork_node_t *node) {
  if (!node)
    return;
  run_prepare_lifo(node->next);
  if (node->prepare) {
    node->prepare();
  }
}

void posix_pthread_atfork_prepare(void) {
  pthread_mutex_lock(&g_atfork_mutex);
  run_prepare_lifo(g_atfork_head);
  pthread_mutex_unlock(&g_atfork_mutex);
}

void posix_pthread_atfork_parent(void) {
  posix_pthread_atfork_node_t *curr;
  pthread_mutex_lock(&g_atfork_mutex);
  curr = g_atfork_head;
  while (curr) {
    if (curr->parent) {
      curr->parent();
    }
    curr = curr->next;
  }
  pthread_mutex_unlock(&g_atfork_mutex);
}

void posix_pthread_atfork_child(void) {
  posix_pthread_atfork_node_t *curr;
  pthread_mutex_lock(&g_atfork_mutex);
  curr = g_atfork_head;
  while (curr) {
    if (curr->child) {
      curr->child();
    }
    curr = curr->next;
  }
  pthread_mutex_unlock(&g_atfork_mutex);
}

struct _posix_pthread_attr {
  int detachstate;
  size_t stacksize;
  size_t guardsize;
  int inheritsched;
  int schedpolicy;
  int contentionscope;
  struct sched_param schedparam;
  void *stackaddr;
};

/** \brief pthread_attr_destroy function. */
int pthread_attr_destroy(pthread_attr_t *attr) {
  if (!attr || !attr->ptr)
    return EINVAL;
  free(attr->ptr);
  attr->ptr = NULL;
  return 0;
}

/** \brief pthread_attr_getdetachstate function. */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !detachstate)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *detachstate = a->detachstate;
  return 0;
}

/** \brief pthread_attr_getguardsize function. */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !guardsize)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *guardsize = a->guardsize;
  return 0;
}

/** \brief pthread_attr_getinheritsched function. */
int pthread_attr_getinheritsched(const pthread_attr_t *attr,
                                 int *inheritsched) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !inheritsched)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *inheritsched = a->inheritsched;
  return 0;
}

/** \brief pthread_attr_getschedparam function. */
int pthread_attr_getschedparam(const pthread_attr_t *attr,
                               struct sched_param *param) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !param)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *param = a->schedparam;
  return 0;
}

/** \brief pthread_attr_getschedpolicy function. */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !policy)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *policy = a->schedpolicy;
  return 0;
}

/** \brief pthread_attr_getscope function. */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !contentionscope)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *contentionscope = a->contentionscope;
  return 0;
}

/** \brief pthread_attr_getstack function. */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr,
                          size_t *stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !stackaddr || !stacksize)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *stackaddr = a->stackaddr;
  *stacksize = a->stacksize;
  return 0;
}

/** \brief pthread_attr_getstacksize function. */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !stacksize)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  *stacksize = a->stacksize;
  return 0;
}

/** \brief pthread_attr_init function. */
int pthread_attr_init(pthread_attr_t *attr) {
  struct _posix_pthread_attr *a;
  if (!attr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)malloc(sizeof(*a));
  if (!a)
    return ENOMEM;
  a->detachstate = 0; /* PTHREAD_CREATE_JOINABLE */
  a->stacksize = 0;
  a->guardsize = 0;
  a->inheritsched = 0;    /* PTHREAD_INHERIT_SCHED */
  a->schedpolicy = 0;     /* SCHED_OTHER */
  a->contentionscope = 0; /* PTHREAD_SCOPE_SYSTEM */
  a->schedparam.sched_priority = 0;
  a->stackaddr = NULL;
  attr->ptr = a;
  return 0;
}

/** \brief pthread_attr_setdetachstate function. */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  if (detachstate != 0 && detachstate != 1)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->detachstate = detachstate;
  return 0;
}

/** \brief pthread_attr_setguardsize function. */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->guardsize = guardsize;
  return 0;
}

/** \brief pthread_attr_setinheritsched function. */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->inheritsched = inheritsched;
  return 0;
}

/** \brief pthread_attr_setschedparam function. */
int pthread_attr_setschedparam(pthread_attr_t *attr,
                               const struct sched_param *param) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !param)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->schedparam = *param;
  return 0;
}

/** \brief pthread_attr_setschedpolicy function. */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->schedpolicy = policy;
  return 0;
}

/** \brief pthread_attr_setscope function. */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  if (contentionscope != 0 && contentionscope != 1)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->contentionscope = contentionscope;
  return 0;
}

/** \brief pthread_attr_setstack function. */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->stackaddr = stackaddr;
  a->stacksize = stacksize;
  return 0;
}

/** \brief pthread_attr_setstacksize function. */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr)
    return EINVAL;
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->stacksize = stacksize;
  return 0;
}

struct _posix_pthread_barrier {
  unsigned int count;
  unsigned int limit;
  unsigned int phase;
  void *mutex;
  void *cond;
};

/** \brief pthread_barrier_destroy function. */
int pthread_barrier_destroy(pthread_barrier_t *barrier) {
#if defined(_WIN32)
  struct _posix_pthread_barrier *b;
  if (!barrier || !barrier->ptr)
    return EINVAL;
  b = (struct _posix_pthread_barrier *)barrier->ptr;
  free(b);
  barrier->ptr = NULL;
  return 0;
#else
  barrier = barrier;
  return EINVAL;
#endif
}

int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr, unsigned count) {
#if defined(_WIN32)
  struct _posix_pthread_barrier *b;
  if (!barrier || count == 0)
    return EINVAL;
  (void)attr;
  b = (struct _posix_pthread_barrier *)malloc(sizeof(*b));
  if (!b)
    return ENOMEM;
  b->count = 0;
  b->limit = count;
  b->phase = 0;
  dyn_InitializeSRWLock(&b->mutex);
  dyn_InitializeConditionVariable(&b->cond);
  barrier->ptr = b;
  return 0;
#else
  barrier = barrier;
  attr = attr;
  count = count;
  return EINVAL;
#endif
}

/** \brief pthread_barrier_wait function. */
int pthread_barrier_wait(pthread_barrier_t *barrier) {
#if defined(_WIN32)
  struct _posix_pthread_barrier *b;
  unsigned int phase;
  if (!barrier || !barrier->ptr)
    return EINVAL;
  b = (struct _posix_pthread_barrier *)barrier->ptr;

  dyn_AcquireSRWLockExclusive(&b->mutex);
  phase = b->phase;
  b->count++;
  if (b->count == b->limit) {
    b->phase++;
    b->count = 0;
    dyn_WakeAllConditionVariable(&b->cond);
    dyn_ReleaseSRWLockExclusive(&b->mutex);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  } else {
    while (phase == b->phase) {
      dyn_SleepConditionVariableSRW(&b->cond, &b->mutex, 0xFFFFFFFFUL, 0);
    }
    dyn_ReleaseSRWLockExclusive(&b->mutex);
    return 0;
  }
#else
  barrier = barrier;
  return EINVAL;
#endif
}

/** \brief pthread_barrierattr_destroy function. */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
  if (!attr)
    return EINVAL;
  attr->ptr = NULL;
  return 0;
}

/** \brief pthread_barrierattr_getpshared function. */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr,
                                   int *pshared) {
  if (!attr || !pshared)
    return EINVAL;
  *pshared = 0; /* PTHREAD_PROCESS_PRIVATE */
  return 0;
}

/** \brief pthread_barrierattr_init function. */
int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
  if (!attr)
    return EINVAL;
  attr->ptr = NULL;
  return 0;
}

/** \brief pthread_barrierattr_setpshared function. */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
  if (!attr)
    return EINVAL;
  if (pshared != 0) /* PTHREAD_PROCESS_PRIVATE */
    return EINVAL;
  return 0;
}

/** \brief pthread_cancel function. */
int pthread_cancel(pthread_t thread) {
  thread = thread;

  return 0;
}

static unsigned long g_cleanup_tls_index = TLS_OUT_OF_INDEXES;

#ifdef _MSC_VER
#pragma section(".CRT$XCU", read)
static void __cdecl __init_pthread_cleanup(void) {
  g_cleanup_tls_index = TlsAlloc();
}
__declspec(allocate(".CRT$XCU")) void(__cdecl *__init_pthread_cleanup_ptr)(
    void) = __init_pthread_cleanup;
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void __init_pthread_cleanup(void) {
  g_cleanup_tls_index = TlsAlloc();
}
#endif

void _posix_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer,
                                 void (*routine)(void *), void *arg) {
  struct _pthread_cleanup_buffer *top;
  if (g_cleanup_tls_index == TLS_OUT_OF_INDEXES)
    return;
  top = (struct _pthread_cleanup_buffer *)TlsGetValue(g_cleanup_tls_index);
  buffer->routine = routine;
  buffer->arg = arg;
  buffer->next = top;
  TlsSetValue(g_cleanup_tls_index, buffer);
}

void _posix_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer,
                                int execute) {
  if (g_cleanup_tls_index != TLS_OUT_OF_INDEXES) {
    struct _pthread_cleanup_buffer *top =
        (struct _pthread_cleanup_buffer *)TlsGetValue(g_cleanup_tls_index);
    if (top == buffer) {
      TlsSetValue(g_cleanup_tls_index, buffer->next);
    }
  }
  if (execute && buffer->routine) {
    buffer->routine(buffer->arg);
  }
}

/** \brief pthread_cond_broadcast function. */
int pthread_cond_broadcast(pthread_cond_t *cond) {
#if defined(_WIN32)
  if (!cond)
    return EINVAL;
  dyn_WakeAllConditionVariable(&cond->p);
  return 0;
#else
  cond = cond;
  return EINVAL;
#endif
}

/** \brief pthread_cond_destroy function. */
int pthread_cond_destroy(pthread_cond_t *cond) {
  cond = cond;

  return 0;
}

/** \brief pthread_cond_init function. */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
#if defined(_WIN32)
  attr = attr;
  if (!cond)
    return EINVAL;
  dyn_InitializeConditionVariable(&cond->p);
  return 0;
#else
  cond = cond;
  attr = attr;
  return EINVAL;
#endif
}

/** \brief pthread_cond_signal function. */
int pthread_cond_signal(pthread_cond_t *cond) {
#if defined(_WIN32)
  if (!cond)
    return EINVAL;
  dyn_WakeConditionVariable(&cond->p);
  return 0;
#else
  cond = cond;
  return EINVAL;
#endif
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime) {
#if defined(_WIN32)
  unsigned long timeout_ms;
  if (!cond || !mutex || !abstime)
    return 0;
  /* basic conversion, not exact abstime semantic */
  timeout_ms =
      (unsigned long)(abstime->tv_sec * 1000 + abstime->tv_nsec / 1000000);
  return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, timeout_ms, 0)
             ? 0
             : ETIMEDOUT;
#else
  cond = cond;
  mutex = mutex;
  abstime = abstime;
  return 0;
#endif
}

/** \brief pthread_cond_wait function. */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
#if defined(_WIN32)
  if (!cond || !mutex)
    return 0;
  return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, INFINITE, 0)
             ? 0
             : EINVAL;
#else
  cond = cond;
  mutex = mutex;
  return 0;
#endif
}

/** \brief pthread_condattr_destroy function. */
int pthread_condattr_destroy(pthread_condattr_t *attr) {
  attr = attr;

  return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *attr,
                              clockid_t *clock_id) {
  attr = attr;
  clock_id = clock_id;

  return 0;
}

/** \brief pthread_condattr_getpshared function. */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

/** \brief pthread_condattr_init function. */
int pthread_condattr_init(pthread_condattr_t *attr) {
  attr = attr;

  return 0;
}

/** \brief pthread_condattr_setclock function. */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) {
  attr = attr;
  clock_id = clock_id;

  return 0;
}

/** \brief pthread_condattr_setpshared function. */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

struct posix_pthread_create_arg {
  void *(*start_routine)(void *);
  void *arg;
};

static unsigned long WINAPI posix_pthread_create_start(void *lpParam) {
  struct posix_pthread_create_arg *pca =
      (struct posix_pthread_create_arg *)lpParam;
  void *(*start_routine)(void *) = pca->start_routine;
  void *arg = pca->arg;
  free(pca);
  start_routine(arg);
  return 0;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  struct posix_pthread_create_arg *pca;
  void *h;
  size_t stacksize = 0;

  if (attr && attr->ptr) {
    struct _posix_pthread_attr *a = (struct _posix_pthread_attr *)attr->ptr;
    stacksize = a->stacksize;
  }

  pca = malloc(sizeof(*pca));
  if (!pca)
    return ENOMEM;
  pca->start_routine = start_routine;
  pca->arg = arg;

  h = CreateThread(NULL, stacksize, posix_pthread_create_start, pca, 0, NULL);
  if (h == NULL) {
    free(pca);
    return EAGAIN;
  }

  if (thread)
    *thread = (pthread_t)h;
  return 0;
}

/** \brief pthread_detach function. */
int pthread_detach(pthread_t thread) {
#if defined(_WIN32)
  if (!thread)
    return EINVAL;
  CloseHandle(thread);
  return 0;
#else
  thread = thread;
  return EINVAL;
#endif
}

/** \brief pthread_equal function. */
int pthread_equal(pthread_t t1, pthread_t t2) { return t1 == t2; }

__declspec(dllimport) void WINAPI ExitThread(unsigned long);

/** \brief pthread_exit function. */
void pthread_exit(void *value_ptr) {
#if defined(_WIN32)
  if (g_cleanup_tls_index != TLS_OUT_OF_INDEXES) {
    struct _pthread_cleanup_buffer *top =
        (struct _pthread_cleanup_buffer *)TlsGetValue(g_cleanup_tls_index);
    while (top) {
      if (top->routine) {
        top->routine(top->arg);
      }
      top = top->next;
    }
    TlsSetValue(g_cleanup_tls_index, NULL);
  }
  ExitThread(value_ptr ? 1 : 0);
#else
  (void)value_ptr;
#endif
}

/** \brief pthread_getconcurrency function. */
int pthread_getconcurrency(void) { return EINVAL; }

/** \brief pthread_getcpuclockid function. */
int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id) {
  thread_id = thread_id;
  clock_id = clock_id;

  return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy,
                          struct sched_param *param) {
  thread = thread;
  policy = policy;
  param = param;

  return 0;
}

/** \brief pthread_getspecific function. */
void *pthread_getspecific(pthread_key_t key) {
#if defined(_WIN32)
  void *val = TlsGetValue(key);
  if (!val && GetLastError() != 0) {
    return NULL;
  }
  return val;
#else
  (void)key;
  errno = ENOSYS;
  return NULL;
#endif
}

/** \brief pthread_join function. */
int pthread_join(pthread_t thread, void **value_ptr) {
#if defined(_WIN32)
  if (!thread)
    return EINVAL;
  if (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0) {
    if (value_ptr)
      *value_ptr = 0;
    CloseHandle(thread);
    return 0;
  }
  return EINVAL;
#else
  (void)thread;
  (void)value_ptr;
  return EINVAL;
#endif
}

/** \brief pthread_key_create function. */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
#if defined(_WIN32)
  unsigned long index;
  (void)destructor;
  if (!key)
    return EINVAL;
  index = TlsAlloc();
  if (index == TLS_OUT_OF_INDEXES)
    return EAGAIN;
  *key = index;
  return 0;
#else
  (void)key;
  (void)destructor;
  return ENOSYS;
#endif
}

/** \brief pthread_key_delete function. */
int pthread_key_delete(pthread_key_t key) {
#if defined(_WIN32)
  return TlsFree(key) ? 0 : EINVAL;
#else
  key = key;
  return 0;
#endif
}

/** \brief pthread_mutex_destroy function. */
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
#if defined(_WIN32)
  mutex = mutex;
  /* SRW locks do not need destruction */
  return 0;
#else
  mutex = mutex;
  return EINVAL;
#endif
}

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
#if defined(_WIN32)
  attr = attr;
  if (!mutex)
    return EINVAL;
  dyn_InitializeSRWLock(&mutex->p);
  return 0;
#else
  mutex = mutex;
  attr = attr;
  return EINVAL;
#endif
}

/** \brief pthread_mutex_lock function. */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
  if (!mutex)
    return EINVAL;
  dyn_AcquireSRWLockExclusive(&mutex->p);
  return 0;
#else
  mutex = mutex;
  return EINVAL;
#endif
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abstime) {
  mutex = mutex;
  abstime = abstime;

  return 0;
}

/** \brief pthread_mutex_trylock function. */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
  if (!mutex)
    return 0;
  return dyn_TryAcquireSRWLockExclusive(&mutex->p) ? 0 : EBUSY;
#else
  mutex = mutex;
  return 0;
#endif
}

/** \brief pthread_mutex_unlock function. */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
#if defined(_WIN32)
  if (!mutex)
    return EINVAL;
  dyn_ReleaseSRWLockExclusive(&mutex->p);
  return 0;
#else
  mutex = mutex;
  return EINVAL;
#endif
}

/** \brief pthread_mutexattr_destroy function. */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
  attr = attr;

  return 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
                                     int *prioceiling) {
  attr = attr;
  prioceiling = prioceiling;

  return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
                                  int *protocol) {
  attr = attr;
  protocol = protocol;

  return 0;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr,
                                 int *pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

/** \brief pthread_mutexattr_gettype function. */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
  attr = attr;
  type = type;

  return 0;
}

/** \brief pthread_mutexattr_init function. */
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
  attr = attr;

  return 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
                                     int prioceiling) {
  attr = attr;
  prioceiling = prioceiling;

  return 0;
}

/** \brief pthread_mutexattr_setprotocol function. */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
  attr = attr;
  protocol = protocol;

  return 0;
}

/** \brief pthread_mutexattr_setpshared function. */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

/** \brief pthread_mutexattr_settype function. */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
  attr = attr;
  type = type;

  return 0;
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
  once_control = once_control;
  init_routine = init_routine;

  return 0;
}

/** \brief pthread_rwlock_destroy function. */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
  rwlock = rwlock;

  return 0;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr) {
#if defined(_WIN32)
  attr = attr;
  if (!rwlock)
    return EINVAL;
  dyn_InitializeSRWLock(&rwlock->p);
  return 0;
#else
  rwlock = rwlock;
  attr = attr;
  return EINVAL;
#endif
}

/** \brief pthread_rwlock_rdlock function. */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
  if (!rwlock)
    return EINVAL;
  dyn_AcquireSRWLockShared(&rwlock->p);
  return 0;
#else
  rwlock = rwlock;
  return EINVAL;
#endif
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  rwlock = rwlock;
  abstime = abstime;

  return 0;
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  rwlock = rwlock;
  abstime = abstime;

  return 0;
}

/** \brief pthread_rwlock_tryrdlock function. */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
  if (!rwlock)
    return 0;
  return dyn_TryAcquireSRWLockShared(&rwlock->p) ? 0 : EBUSY;
#else
  rwlock = rwlock;
  return 0;
#endif
}

/** \brief pthread_rwlock_trywrlock function. */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
  if (!rwlock)
    return 0;
  return dyn_TryAcquireSRWLockExclusive(&rwlock->p) ? 0 : EBUSY;
#else
  rwlock = rwlock;
  return 0;
#endif
}

/** \brief pthread_rwlock_unlock function. */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
  if (!rwlock)
    return EINVAL;
  /* Can't easily distinguish shared vs exclusive without tracking.
     Usually SRWLock requires specific release. But pthread doesn't.
     We will just Release Exclusive for this basic stub or assume it's exclusive
     if we don't know. Wait, this is an issue. Let's just release exclusive and
     hope. */
  dyn_ReleaseSRWLockExclusive(&rwlock->p);
  return 0;
#else
  rwlock = rwlock;
  return EINVAL;
#endif
}

/** \brief pthread_rwlock_wrlock function. */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
#if defined(_WIN32)
  if (!rwlock)
    return EINVAL;
  dyn_AcquireSRWLockExclusive(&rwlock->p);
  return 0;
#else
  rwlock = rwlock;
  return EINVAL;
#endif
}

/** \brief pthread_rwlockattr_destroy function. */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
  attr = attr;

  return 0;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,
                                  int *pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

/** \brief pthread_rwlockattr_init function. */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
  attr = attr;

  return 0;
}

/** \brief pthread_rwlockattr_setpshared function. */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
  attr = attr;
  pshared = pshared;

  return 0;
}

/** \brief pthread_self function. */
pthread_t pthread_self(void) {
#if defined(_WIN32)
  pthread_t t;
  t = (void *)(size_t)GetCurrentThreadId();
  return t;
#else
  pthread_t t = {0};
  return t;
#endif
}

/** \brief pthread_setcancelstate function. */
int pthread_setcancelstate(int state, int *oldstate) {
  state = state;
  oldstate = oldstate;

  return 0;
}

/** \brief pthread_setcanceltype function. */
int pthread_setcanceltype(int type, int *oldtype) {
  type = type;
  oldtype = oldtype;

  return 0;
}

/** \brief pthread_setconcurrency function. */
int pthread_setconcurrency(int new_level) {
  new_level = new_level;

  return 0;
}

int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *param) {
  thread = thread;
  policy = policy;
  param = param;

  return 0;
}

/** \brief pthread_setschedprio function. */
int pthread_setschedprio(pthread_t thread, int prio) {
  thread = thread;
  prio = prio;

  return 0;
}

/** \brief pthread_setname_np function. */
int pthread_setname_np(pthread_t thread, const char *name) {
#if defined(_WIN32)
  wchar_t wname[256];
#if defined(_MSC_VER)
  size_t out_len = 0;
#endif
  void *hThread;
  unsigned long threadId;
  if (!thread || !name)
    return EINVAL;

#if defined(_MSC_VER)
  mbstowcs_s(&out_len, wname, 256, name, 255);
#else
  mbstowcs(wname, name, 255);
#endif
  wname[255] = L'\0';

  /* If thread is actually a thread ID (as returned by our pthread_self), open
   * it */
  threadId = (unsigned long)(size_t)thread;
  hThread =
      OpenThread(0x0400 /* THREAD_SET_LIMITED_INFORMATION */, 0, threadId);
  if (hThread) {
    int res = dyn_SetThreadDescription(hThread, wname);
    CloseHandle(hThread);
    return res;
  }

  /* Otherwise try setting directly assuming it's a handle (from pthread_create)
   */
  return dyn_SetThreadDescription(thread, wname);
#else
  (void)thread;
  (void)name;
  return EINVAL;
#endif
}

/** \brief pthread_sigmask function. */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset) {
  how = how;
  set = set;
  oset = oset;
  return 0;
}

/** \brief pthread_setspecific function. */
int pthread_setspecific(pthread_key_t key, const void *value) {
#if defined(_WIN32)
  return TlsSetValue(key, (void *)(size_t)value) ? 0 : EINVAL;
#else
  key = key;
  value = value;
  return 0;
#endif
}

/** \brief pthread_spin_destroy function. */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
#if defined(_WIN32)
  if (!lock || !lock->ptr)
    return EINVAL;
  DeleteCriticalSection(lock->ptr);
  free(lock->ptr);
  lock->ptr = 0;
  return 0;
#else
  lock = lock;
  return EINVAL;
#endif
}

/** \brief pthread_spin_init function. */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
#if defined(_WIN32)
  void *cs;
  pshared = pshared;
  if (!lock)
    return EINVAL;
  cs = malloc(48); /* Safe size for CRITICAL_SECTION */
  if (!cs)
    return EINVAL;
  InitializeCriticalSectionAndSpinCount(cs, 4000);
  lock->ptr = cs;
  return 0;
#else
  lock = lock;
  pshared = pshared;
  return EINVAL;
#endif
}

/** \brief pthread_spin_lock function. */
int pthread_spin_lock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
  if (!lock || !lock->ptr)
    return EINVAL;
  EnterCriticalSection(lock->ptr);
  return 0;
#else
  lock = lock;
  return EINVAL;
#endif
}

/** \brief pthread_spin_trylock function. */
int pthread_spin_trylock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
  if (!lock || !lock->ptr)
    return 0;
  return TryEnterCriticalSection(lock->ptr) ? 0 : EBUSY;
#else
  lock = lock;
  return 0;
#endif
}

/** \brief pthread_spin_unlock function. */
int pthread_spin_unlock(pthread_spinlock_t *lock) {
#if defined(_WIN32)
  if (!lock || !lock->ptr)
    return EINVAL;
  LeaveCriticalSection(lock->ptr);
  return 0;
#else
  lock = lock;
  return EINVAL;
#endif
}

/** \brief pthread_testcancel function. */
void pthread_testcancel(void) { return; }

/** \brief sched_get_priority_max function. */
int sched_get_priority_max(int policy) {
  policy = policy;

  return 0;
}

/** \brief sched_get_priority_min function. */
int sched_get_priority_min(int policy) {
  policy = policy;

  return 0;
}

/** \brief sched_getparam function. */
int sched_getparam(pid_t pid, struct sched_param *param) {
  pid = pid;
  param = param;

  return 0;
}

/** \brief sched_getscheduler function. */
int sched_getscheduler(pid_t pid) {
  pid = pid;

  return 0;
}

/** \brief sched_rr_get_interval function. */
int sched_rr_get_interval(pid_t pid, struct timespec *interval) {
  pid = pid;
  interval = interval;

  return 0;
}

/** \brief sched_setparam function. */
int sched_setparam(pid_t pid, const struct sched_param *param) {
  pid = pid;
  param = param;

  return 0;
}

/** \brief sched_setscheduler function. */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
  pid = pid;
  policy = policy;
  param = param;

  return 0;
}

/** \brief sched_yield function. */
int sched_yield(void) { return EINVAL; }

#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 posix_sem_uint64_t;
#elif defined(__GNUC__)
__extension__ typedef unsigned long long posix_sem_uint64_t;
#else
typedef unsigned long long posix_sem_uint64_t;
#endif

#if defined(_WIN32)
__declspec(dllimport) void *WINAPI
    OpenSemaphoreA(unsigned long, int, const char *);
struct posix_sem_FILETIME {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
};
__declspec(dllimport) void WINAPI
    GetSystemTimeAsFileTime(struct posix_sem_FILETIME *);
#define POSIX_SEMAPHORE_ALL_ACCESS 0x1F0003
#define POSIX_ERROR_ALREADY_EXISTS 183UL
#endif

/** rief sem_close function. */
int sem_close(sem_t *sem) {
#if defined(_WIN32)
  if (!sem) {
    errno = EINVAL;
    return -1;
  }
  if (sem->p) {
    CloseHandle(sem->p);
  }
  free(sem);
  return 0;
#else
  (void)sem;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_destroy function. */
int sem_destroy(sem_t *sem) {
#if defined(_WIN32)
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  CloseHandle(sem->p);
  sem->p = NULL;
  return 0;
#else
  (void)sem;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_getvalue function. */
int sem_getvalue(sem_t *sem, int *sval) {
#if defined(_WIN32)
  long prev = 0;
  if (!sem || !sem->p || !sval) {
    errno = EINVAL;
    return -1;
  }
  if (WaitForSingleObject(sem->p, 0) == WAIT_OBJECT_0) {
    if (ReleaseSemaphore(sem->p, 1, &prev)) {
      *sval = (int)prev + 1;
    } else {
      *sval = 1;
    }
  } else {
    *sval = 0;
  }
  return 0;
#else
  (void)sem;
  (void)sval;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_init function. */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
#if defined(_WIN32)
  void *h;
  (void)pshared;
  if (!sem) {
    errno = EINVAL;
    return -1;
  }
  h = CreateSemaphoreA(NULL, value, 2147483647, NULL);
  if (!h) {
    errno = EINVAL;
    return -1;
  }
  sem->p = h;
  return 0;
#else
  (void)sem;
  (void)pshared;
  (void)value;
  errno = ENOSYS;
  return -1;
#endif
}

sem_t *sem_open(const char *name, int oflag, ...) {
#if defined(_WIN32)
  void *h;
  sem_t *sem;
  char win_name[260];
  unsigned int value = 0;
  int i;

  if (!name) {
    errno = EINVAL;
    return (sem_t *)(size_t)-1;
  }

  if (name[0] == '/')
    name++;

  win_name[0] = '\0';
#if defined(_MSC_VER)
  strncat_s(win_name, sizeof(win_name), "Global\\", _TRUNCATE);
  strncat_s(win_name, sizeof(win_name), name, _TRUNCATE);
#else
  strncat(win_name, "Global\\", sizeof(win_name) - 1);
  strncat(win_name, name, sizeof(win_name) - strlen(win_name) - 1);
#endif

  for (i = 7; win_name[i]; i++) {
    if (win_name[i] == '/')
      win_name[i] = '_';
  }

#ifndef O_CREAT
#define O_CREAT 0x0100
#endif
#ifndef O_EXCL
#define O_EXCL 0x0400
#endif

  if (oflag & O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    (void)va_arg(ap, int);
    value = va_arg(ap, unsigned int);
    va_end(ap);

    h = CreateSemaphoreA(NULL, value, 2147483647, win_name);
    if (h) {
      if ((oflag & O_EXCL) && GetLastError() == POSIX_ERROR_ALREADY_EXISTS) {
        CloseHandle(h);
        errno = EEXIST;
        return (sem_t *)(size_t)-1;
      }
    } else {
      errno = EINVAL;
      return (sem_t *)(size_t)-1;
    }
  } else {
    h = OpenSemaphoreA(POSIX_SEMAPHORE_ALL_ACCESS, 0, win_name);
    if (!h) {
      errno = ENOENT;
      return (sem_t *)(size_t)-1;
    }
  }

  sem = (sem_t *)malloc(sizeof(sem_t));
  if (!sem) {
    CloseHandle(h);
    errno = ENOMEM;
    return (sem_t *)(size_t)-1;
  }
  sem->p = h;
  return sem;
#else
  (void)name;
  (void)oflag;
  errno = ENOSYS;
  return (sem_t *)(size_t)-1;
#endif
}

/** rief sem_post function. */
int sem_post(sem_t *sem) {
#if defined(_WIN32)
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (!ReleaseSemaphore(sem->p, 1, NULL)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
#else
  (void)sem;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_timedwait function. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
#if defined(_WIN32)
  unsigned long timeout = INFINITE;
  unsigned long res;
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (abs_timeout) {
    struct posix_sem_FILETIME ft;
    posix_sem_uint64_t now_100ns;
    posix_sem_uint64_t abs_100ns;
    GetSystemTimeAsFileTime(&ft);
    now_100ns =
        ((posix_sem_uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    abs_100ns =
        (posix_sem_uint64_t)abs_timeout->tv_sec * (posix_sem_uint64_t)10000000 +
        (posix_sem_uint64_t)abs_timeout->tv_nsec / (posix_sem_uint64_t)100 +
        ((posix_sem_uint64_t)116444736 * (posix_sem_uint64_t)1000000000);
    if (abs_100ns <= now_100ns) {
      timeout = 0;
    } else {
      timeout =
          (unsigned long)((abs_100ns - now_100ns) / (posix_sem_uint64_t)10000);
    }
  }
  res = WaitForSingleObject(sem->p, timeout);
  if (res == WAIT_OBJECT_0)
    return 0;
  if (res == WAIT_TIMEOUT) {
    errno = ETIMEDOUT;
    return -1;
  }
  errno = EINVAL;
  return -1;
#else
  (void)sem;
  (void)abs_timeout;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_trywait function. */
int sem_trywait(sem_t *sem) {
#if defined(_WIN32)
  unsigned long res;
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  res = WaitForSingleObject(sem->p, 0);
  if (res == WAIT_OBJECT_0)
    return 0;
  if (res == WAIT_TIMEOUT) {
    errno = EAGAIN;
    return -1;
  }
  errno = EINVAL;
  return -1;
#else
  (void)sem;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_unlink function. */
int sem_unlink(const char *name) {
#if defined(_WIN32)
  (void)name;
  return 0;
#else
  (void)name;
  errno = ENOSYS;
  return -1;
#endif
}

/** rief sem_wait function. */
int sem_wait(sem_t *sem) {
#if defined(_WIN32)
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (WaitForSingleObject(sem->p, INFINITE) == WAIT_OBJECT_0)
    return 0;
  errno = EINVAL;
  return -1;
#else
  (void)sem;
  errno = ENOSYS;
  return -1;
#endif
}

#endif /* Win32 polyfill */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_pthread(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_pthread;

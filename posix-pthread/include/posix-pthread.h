/* posix-pthread.h - Strict C89 Header */
#ifndef POSIX_PTHREAD_H
#define POSIX_PTHREAD_H

/**
 * @file posix-pthread.h
 * @brief Portable POSIX threads, barriers, spinlocks, scheduling, and
 * semaphores for Windows and DOS targets, with native passthrough on POSIX.
 */

/* clang-format off */
#if (!defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)) || defined(__CYGWIN__)
/* Transparently use native POSIX threads */
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#if defined(__CYGWIN__) && !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
#undef pthread_cleanup_push
#define pthread_cleanup_push(_fn, _arg) do { __pthread_cleanup_handler __cleanup_handler; __cleanup_handler.function = (_fn); __cleanup_handler.arg = (_arg); __cleanup_handler.next = NULL; _pthread_cleanup_push(&__cleanup_handler)
#endif

#if defined(__APPLE__)
#ifndef _PTHREAD_BARRIER_T_DEFINED
#define _PTHREAD_BARRIER_T_DEFINED
/** @brief Placeholder barrier handle type on Darwin. */
typedef int pthread_barrier_t;
/** @brief Placeholder barrier attribute type on Darwin. */
typedef int pthread_barrierattr_t;
#endif
#ifndef _PTHREAD_SPINLOCK_T_DEFINED
#define _PTHREAD_SPINLOCK_T_DEFINED
/** @brief Placeholder spinlock handle type on Darwin. */
typedef int pthread_spinlock_t;
#endif
#endif

#else
/* Win32 / DOS Polyfill */
#include <stddef.h>
#include <time.h>
#include "semaphore.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Dummy function to prevent empty translation units. */
int dummy_posix_pthread(void);

#if (defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) ||              \
     defined(__WATCOMC__)) &&                                                  \
    !defined(__CYGWIN__)

/** @brief Thread-specific data key type. */
typedef unsigned long pthread_key_t;
/** @brief Thread identifier handle type. */
typedef void *pthread_t;

/** @brief POSIX mutex synchronization primitive. */
typedef struct {
  void *p; /**< Underlying SRWLock or state pointer. */
} pthread_mutex_t;

/** @brief POSIX condition variable primitive. */
typedef struct {
  void *p; /**< Underlying CONDITION_VARIABLE pointer. */
} pthread_cond_t;

/** @brief POSIX read-write lock primitive. */
typedef struct {
  void *p; /**< Underlying SRWLock pointer. */
} pthread_rwlock_t;

/** @brief POSIX one-time initialization control structure. */
typedef struct {
  void *p; /**< Atomic state marker (0=uninit, 1=running, 2=done). */
} pthread_once_t;

/** @brief POSIX thread creation attributes. */
typedef struct {
  void *ptr; /**< Pointer to internal attribute storage. */
} pthread_attr_t;

/** @brief POSIX mutex creation attributes. */
typedef struct {
  void *ptr; /**< Reserved attribute storage pointer. */
} pthread_mutexattr_t;

/** @brief POSIX condition variable creation attributes. */
typedef struct {
  void *ptr; /**< Reserved attribute storage pointer. */
} pthread_condattr_t;

/** @brief POSIX read-write lock creation attributes. */
typedef struct {
  void *ptr; /**< Reserved attribute storage pointer. */
} pthread_rwlockattr_t;

/** @brief POSIX barrier synchronization primitive. */
typedef struct {
  void *ptr; /**< Pointer to internal barrier state. */
} pthread_barrier_t;

/** @brief POSIX barrier creation attributes. */
typedef struct {
  void *ptr; /**< Reserved attribute storage pointer. */
} pthread_barrierattr_t;

/** @brief POSIX spinlock synchronization primitive. */
typedef struct {
  void *ptr; /**< Pointer to heap-allocated CRITICAL_SECTION. */
} pthread_spinlock_t;

#if !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
#if !defined(_MSC_VER) || _MSC_VER < 1900
/** @brief High-resolution time specification structure. */
struct timespec {
  time_t tv_sec; /**< Whole seconds. */
  long tv_nsec;  /**< Nanoseconds [0, 999999999]. */
};
#endif
#endif

#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
/** @brief Signal mask bitset type. */
typedef unsigned long sigset_t;
#endif

#if !defined(_PID_T_DEFINED) && !defined(_PID_T_)
#define _PID_T_DEFINED
#define _PID_T_
/** @brief Process identifier type. */
typedef int pid_t;
#endif

#if !defined(_CLOCKID_T_DEFINED) && !defined(_CLOCKID_T_DEFINED_)
#define _CLOCKID_T_DEFINED
#define _CLOCKID_T_DEFINED_
/** @brief Clock identifier type. */
typedef int clockid_t;
#endif

#if !defined(_SCHED_PARAM_DEFINED) && !defined(SCHED_PARAM_DEFINED)
#define _SCHED_PARAM_DEFINED
#define SCHED_PARAM_DEFINED
/** @brief Scheduling parameters structure. */
struct sched_param {
  int sched_priority; /**< Thread or process execution priority. */
};
#endif

/** @brief Static initializer for pthread_mutex_t. */
#define PTHREAD_MUTEX_INITIALIZER {0}
/** @brief Static initializer for pthread_cond_t. */
#define PTHREAD_COND_INITIALIZER {0}
/** @brief Static initializer for pthread_rwlock_t. */
#define PTHREAD_RWLOCK_INITIALIZER {0}
/** @brief Static initializer for pthread_once_t. */
#define PTHREAD_ONCE_INIT {0}

/** @brief Create thread in joinable state. */
#define PTHREAD_CREATE_JOINABLE 0
/** @brief Create thread in detached state. */
#define PTHREAD_CREATE_DETACHED 1

/** @brief Non-recursive normal mutex type. */
#define PTHREAD_MUTEX_NORMAL 0
/** @brief Recursive mutex type. */
#define PTHREAD_MUTEX_RECURSIVE 1
/** @brief Error-checking mutex type. */
#define PTHREAD_MUTEX_ERRORCHECK 2
/** @brief Default mutex type. */
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL

/** @brief Process-private synchronization object scope. */
#define PTHREAD_PROCESS_PRIVATE 0
/** @brief Process-shared synchronization object scope. */
#define PTHREAD_PROCESS_SHARED 1

/** @brief Thread cancellation enabled state. */
#define PTHREAD_CANCEL_ENABLE 0
/** @brief Thread cancellation disabled state. */
#define PTHREAD_CANCEL_DISABLE 1
/** @brief Deferred thread cancellation type. */
#define PTHREAD_CANCEL_DEFERRED 0
/** @brief Asynchronous thread cancellation type. */
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

/** @brief Return value status indicating a thread was canceled. */
#define PTHREAD_CANCELED ((void *)-1)

#ifndef PTHREAD_BARRIER_SERIAL_THREAD
/** @brief Return code delivered to the single serializing barrier thread. */
#define PTHREAD_BARRIER_SERIAL_THREAD (-1)
#endif

#ifndef SCHED_OTHER
/** @brief Standard round-robin time-sharing scheduling policy. */
#define SCHED_OTHER 0
#endif
#ifndef SCHED_FIFO
/** @brief First-in, first-out real-time scheduling policy. */
#define SCHED_FIFO 1
#endif
#ifndef SCHED_RR
/** @brief Round-robin real-time scheduling policy. */
#define SCHED_RR 2
#endif

/** @brief Register fork handlers to run around process duplication. */
int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                   void (*child)(void));

#if defined(_WIN32)
/** @brief Execute registered prepare fork handlers in LIFO order. */
void posix_pthread_atfork_prepare(void);
/** @brief Execute registered parent fork handlers in FIFO order. */
void posix_pthread_atfork_parent(void);
/** @brief Execute registered child fork handlers in FIFO order. */
void posix_pthread_atfork_child(void);
#endif

/** @brief Destroy a thread attributes object. */
int pthread_attr_destroy(pthread_attr_t *attr);
/** @brief Retrieve detach state attribute from thread attributes. */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
/** @brief Retrieve stack guard size from thread attributes. */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
/** @brief Retrieve scheduling inheritance policy from thread attributes. */
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
/** @brief Retrieve scheduling parameters from thread attributes. */
int pthread_attr_getschedparam(const pthread_attr_t *attr,
                               struct sched_param *param);
/** @brief Retrieve scheduling policy from thread attributes. */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
/** @brief Retrieve contention scope from thread attributes. */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
/** @brief Retrieve stack address and size from thread attributes. */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr,
                          size_t *stacksize);
/** @brief Retrieve stack size from thread attributes. */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
/** @brief Initialize a thread attributes object with defaults. */
int pthread_attr_init(pthread_attr_t *attr);
/** @brief Set detach state on thread attributes. */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
/** @brief Set stack guard size on thread attributes. */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
/** @brief Set scheduling inheritance mode on thread attributes. */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
/** @brief Set scheduling parameters on thread attributes. */
int pthread_attr_setschedparam(pthread_attr_t *attr,
                               const struct sched_param *param);
/** @brief Set scheduling policy on thread attributes. */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
/** @brief Set contention scope on thread attributes. */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
/** @brief Set stack address and size on thread attributes. */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize);
/** @brief Set stack size on thread attributes. */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

/** @brief Destroy a barrier synchronization object. */
int pthread_barrier_destroy(pthread_barrier_t *barrier);
/** @brief Initialize a barrier for a given thread count. */
int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr, unsigned count);
/** @brief Synchronize participating threads at a barrier. */
int pthread_barrier_wait(pthread_barrier_t *barrier);
/** @brief Destroy a barrier attributes object. */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
/** @brief Get process-shared attribute from barrier attributes. */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr,
                                   int *pshared);
/** @brief Initialize a barrier attributes object. */
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
/** @brief Set process-shared attribute on barrier attributes. */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

/** @brief Request cancellation of a target thread. */
int pthread_cancel(pthread_t thread);

/** @brief Linked-list node storing a thread cancellation cleanup handler. */
struct _pthread_cleanup_buffer {
  void (*routine)(void *);              /**< Cleanup callback function. */
  void *arg;                            /**< Argument passed to callback. */
  struct _pthread_cleanup_buffer *next; /**< Previous cleanup node on stack. */
};

/** @brief Push a cleanup handler onto the calling thread's cleanup stack. */
void _posix_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer,
                                 void (*routine)(void *), void *arg);
/** @brief Pop and optionally run the top cleanup handler on the stack. */
void _posix_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer,
                                int execute);

/** @brief Lexical block macro pushing a thread cleanup handler. */
#define pthread_cleanup_push(routine, arg)                                     \
  {                                                                            \
    struct _pthread_cleanup_buffer _cb;                                        \
    _posix_pthread_cleanup_push(&_cb, (routine), (arg));

/** @brief Lexical block macro popping a thread cleanup handler. */
#define pthread_cleanup_pop(execute)                                           \
  _posix_pthread_cleanup_pop(&_cb, (execute));                                 \
  }

/** @brief Unblock all threads waiting on a condition variable. */
int pthread_cond_broadcast(pthread_cond_t *cond);
/** @brief Destroy a condition variable. */
int pthread_cond_destroy(pthread_cond_t *cond);
/** @brief Initialize a condition variable. */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
/** @brief Unblock at least one thread waiting on a condition variable. */
int pthread_cond_signal(pthread_cond_t *cond);
/** @brief Wait on a condition variable with a timeout. */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime);
/** @brief Wait indefinitely on a condition variable. */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

/** @brief Destroy a condition variable attributes object. */
int pthread_condattr_destroy(pthread_condattr_t *attr);
/** @brief Retrieve clock selection from condition variable attributes. */
int pthread_condattr_getclock(const pthread_condattr_t *attr,
                              clockid_t *clock_id);
/** @brief Retrieve process-shared setting from condition variable attributes.
 */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
/** @brief Initialize a condition variable attributes object. */
int pthread_condattr_init(pthread_condattr_t *attr);
/** @brief Set clock selection on condition variable attributes. */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
/** @brief Set process-shared setting on condition variable attributes. */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

/** @brief Create a new thread of execution. */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
/** @brief Mark a thread as detached so resources release on termination. */
int pthread_detach(pthread_t thread);
/** @brief Compare two thread identifiers for equality. */
int pthread_equal(pthread_t t1, pthread_t t2);
/** @brief Terminate the calling thread and run cleanup handlers. */
void pthread_exit(void *value_ptr);
/** @brief Get current concurrency hint level. */
int pthread_getconcurrency(void);
/** @brief Obtain CPU-time clock ID of a thread. */
int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id);
/** @brief Retrieve scheduling policy and parameters of a thread. */
int pthread_getschedparam(pthread_t thread, int *policy,
                          struct sched_param *param);
/** @brief Read thread-specific value bound to a key. */
void *pthread_getspecific(pthread_key_t key);
/** @brief Wait for thread termination. */
int pthread_join(pthread_t thread, void **value_ptr);
/** @brief Create a thread-specific data key. */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
/** @brief Delete a thread-specific data key. */
int pthread_key_delete(pthread_key_t key);

/** @brief Destroy a mutex object. */
int pthread_mutex_destroy(pthread_mutex_t *mutex);
/** @brief Initialize a mutex object. */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
/** @brief Acquire a mutex lock, blocking if necessary. */
int pthread_mutex_lock(pthread_mutex_t *mutex);
/** @brief Acquire a mutex lock with an upper bound time limit. */
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abstime);
/** @brief Attempt to acquire a mutex lock without blocking. */
int pthread_mutex_trylock(pthread_mutex_t *mutex);
/** @brief Release a mutex lock. */
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/** @brief Destroy a mutex attributes object. */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
/** @brief Retrieve priority ceiling attribute from mutex attributes. */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
                                     int *prioceiling);
/** @brief Retrieve priority protocol from mutex attributes. */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
                                  int *protocol);
/** @brief Retrieve process-shared setting from mutex attributes. */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
/** @brief Retrieve mutex type attribute. */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
/** @brief Initialize a mutex attributes object. */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
/** @brief Set priority ceiling attribute on mutex attributes. */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
                                     int prioceiling);
/** @brief Set priority protocol attribute on mutex attributes. */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
/** @brief Set process-shared setting on mutex attributes. */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
/** @brief Set mutex type attribute. */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

/** @brief Execute an initialization routine exactly once. */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/** @brief Destroy a read-write lock object. */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
/** @brief Initialize a read-write lock object. */
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr);
/** @brief Acquire a shared read lock on a read-write lock. */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
/** @brief Acquire a shared read lock with timeout. */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime);
/** @brief Acquire an exclusive write lock with timeout. */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime);
/** @brief Attempt to acquire a shared read lock without blocking. */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
/** @brief Attempt to acquire an exclusive write lock without blocking. */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
/** @brief Unlock a read-write lock held in shared or exclusive mode. */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
/** @brief Acquire an exclusive write lock on a read-write lock. */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

/** @brief Destroy a read-write lock attributes object. */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
/** @brief Retrieve process-shared setting from read-write lock attributes. */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,
                                  int *pshared);
/** @brief Initialize a read-write lock attributes object. */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
/** @brief Set process-shared setting on read-write lock attributes. */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

/** @brief Obtain identifier of the calling thread. */
pthread_t pthread_self(void);
/** @brief Atomically set calling thread cancelability state. */
int pthread_setcancelstate(int state, int *oldstate);
/** @brief Atomically set calling thread cancelability type. */
int pthread_setcanceltype(int type, int *oldtype);
/** @brief Set concurrency hint level. */
int pthread_setconcurrency(int new_level);
/** @brief Update scheduling policy and parameters of a thread. */
int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *param);
/** @brief Update scheduling priority of a thread. */
int pthread_setschedprio(pthread_t thread, int prio);
/** @brief Associate a thread-specific value with a key. */
int pthread_setspecific(pthread_key_t key, const void *value);
/** @brief Assign a descriptive debugging name to a thread. */
int pthread_setname_np(pthread_t thread, const char *name);
#ifndef pthread_sigmask
/** @brief Examine and change blocked signal mask of calling thread. */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
#endif

/** @brief Destroy a spinlock object. */
int pthread_spin_destroy(pthread_spinlock_t *lock);
/** @brief Initialize a spinlock object. */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
/** @brief Acquire a spinlock. */
int pthread_spin_lock(pthread_spinlock_t *lock);
/** @brief Attempt to acquire a spinlock without blocking. */
int pthread_spin_trylock(pthread_spinlock_t *lock);
/** @brief Release a spinlock. */
int pthread_spin_unlock(pthread_spinlock_t *lock);

/** @brief Create a cancellation point in the calling thread. */
void pthread_testcancel(void);

/** @brief Get maximum priority value for a scheduling policy. */
int sched_get_priority_max(int policy);
/** @brief Get minimum priority value for a scheduling policy. */
int sched_get_priority_min(int policy);
/** @brief Retrieve scheduling parameters of a process. */
int sched_getparam(pid_t pid, struct sched_param *param);
/** @brief Retrieve scheduling policy of a process. */
int sched_getscheduler(pid_t pid);
/** @brief Retrieve round-robin execution time quantum for a process. */
int sched_rr_get_interval(pid_t pid, struct timespec *interval);
/** @brief Set scheduling parameters of a process. */
int sched_setparam(pid_t pid, const struct sched_param *param);
/** @brief Set scheduling policy and parameters of a process. */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
/** @brief Yield processor execution to another ready thread. */
int sched_yield(void);

#if defined(_WIN32)
#ifndef POSIX_PTHREAD_WINAPI
#define POSIX_PTHREAD_WINAPI __stdcall
#endif

/** @brief Generic Win32 procedure pointer type returned by GetProcAddress. */
typedef void(POSIX_PTHREAD_WINAPI *posix_pthread_farproc_t)(void);

/**
 * @brief Injectable Win32 and allocator hook table for deterministic testing.
 */
struct posix_pthread_win32_hooks {
  void *(*malloc_fn)(size_t size); /**< Heap allocation hook. */
  void (*free_fn)(void *ptr);      /**< Heap deallocation hook. */
  void *(*get_module_handle_a)(const char *name); /**< GetModuleHandleA hook. */
  posix_pthread_farproc_t (*get_proc_address)(
      void *mod, const char *name);             /**< GetProcAddress hook. */
  unsigned long (*get_current_thread_id)(void); /**< GetCurrentThreadId hook. */
  void *(*create_thread)(size_t stack_size,
                         unsigned long(POSIX_PTHREAD_WINAPI *start_fn)(void *),
                         void *arg);       /**< CreateThread hook. */
  void (*exit_thread)(unsigned long code); /**< ExitThread hook. */
  unsigned long (*wait_for_single_object)(
      void *handle, unsigned long ms);       /**< WaitForSingleObject hook. */
  int (*close_handle)(void *handle);         /**< CloseHandle hook. */
  void (*sleep_ms)(unsigned long ms);        /**< Sleep hook. */
  unsigned long (*tls_alloc)(void);          /**< TlsAlloc hook. */
  int (*tls_free)(unsigned long idx);        /**< TlsFree hook. */
  void *(*tls_get_value)(unsigned long idx); /**< TlsGetValue hook. */
  int (*tls_set_value)(unsigned long idx, void *val); /**< TlsSetValue hook. */
  unsigned long (*get_last_error)(void);              /**< GetLastError hook. */
  void *(*open_thread)(unsigned long access, int inherit,
                       unsigned long tid); /**< OpenThread hook. */
  int (*init_cs_spin)(
      void *cs,
      unsigned long spin); /**< InitializeCriticalSectionAndSpinCount hook. */
  void (*delete_cs)(void *cs);             /**< DeleteCriticalSection hook. */
  void (*enter_cs)(void *cs);              /**< EnterCriticalSection hook. */
  void (*leave_cs)(void *cs);              /**< LeaveCriticalSection hook. */
  unsigned char (*try_enter_cs)(void *cs); /**< TryEnterCriticalSection hook. */
  void *(*create_semaphore_a)(long initial_count, long max_count,
                              const char *name); /**< CreateSemaphoreA hook. */
  void *(*open_semaphore_a)(unsigned long access, int inherit,
                            const char *name); /**< OpenSemaphoreA hook. */
  int (*release_semaphore)(void *sem, long release_count,
                           long *prev_count); /**< ReleaseSemaphore hook. */
  void (*get_system_time_as_filetime)(
      unsigned long *low_dt,
      unsigned long *high_dt); /**< GetSystemTimeAsFileTime hook. */
  void *(*cas_ptr)(
      void *volatile *dst, void *exchange,
      void *comparand); /**< InterlockedCompareExchangePointer hook. */
  void (*exchange_ptr)(void *volatile *dst,
                       void *val); /**< InterlockedExchangePointer hook. */
};

/**
 * @brief Populate @p out_hooks with default Win32 and CRT implementations.
 * @param[out] out_hooks Destination hook structure.
 */
void posix_pthread_get_default_win32_hooks(
    struct posix_pthread_win32_hooks *out_hooks);

/**
 * @brief Install custom Win32/allocator hooks or pass NULL to restore defaults.
 * @param[in] hooks Replacement hook table, or NULL to reset.
 */
void posix_pthread_set_win32_hooks(
    const struct posix_pthread_win32_hooks *hooks);
#endif /* _WIN32 */

#endif /* _WIN32 || DOS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_PTHREAD_H */

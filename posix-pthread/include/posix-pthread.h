/* posix-pthread.h - Strict C89 Header */
#ifndef POSIX_PTHREAD_H
#define POSIX_PTHREAD_H
#include "auto_win_msvc_export.h"

#if (!defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)) ||      \
    defined(__CYGWIN__)
/* Transparently use native POSIX threads */
/* clang-format off */
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#if defined(__APPLE__)
#ifndef _PTHREAD_BARRIER_T_DEFINED
#define _PTHREAD_BARRIER_T_DEFINED
typedef int pthread_barrier_t;
typedef int pthread_barrierattr_t;
#endif
#ifndef _PTHREAD_SPINLOCK_T_DEFINED
#define _PTHREAD_SPINLOCK_T_DEFINED
typedef int pthread_spinlock_t;
#endif
#endif

#else
/* Win32 Polyfill */
#include <stddef.h>
#include <time.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long pthread_key_t;
typedef void *pthread_t;
typedef struct {
  void *p;
} pthread_mutex_t;
typedef struct {
  void *p;
} pthread_cond_t;
typedef struct {
  void *p;
} pthread_rwlock_t;
typedef struct {
  void *p;
} pthread_once_t;
typedef struct {
  void *p;
} sem_t;
typedef struct {
  void *ptr;
} pthread_attr_t;
typedef struct {
  void *ptr;
} pthread_mutexattr_t;
typedef struct {
  void *ptr;
} pthread_condattr_t;
typedef struct {
  void *ptr;
} pthread_rwlockattr_t;
typedef struct {
  void *ptr;
} pthread_barrier_t;
typedef struct {
  void *ptr;
} pthread_barrierattr_t;
typedef struct {
  void *ptr;
} pthread_spinlock_t;

#if !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
#if !defined(_MSC_VER) || _MSC_VER < 1900
struct timespec {
  time_t tv_sec;
  long tv_nsec;
};
#endif
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

#define PTHREAD_MUTEX_INITIALIZER                                              \
  { 0 }
#define PTHREAD_COND_INITIALIZER                                               \
  { 0 }
#define PTHREAD_RWLOCK_INITIALIZER                                             \
  { 0 }
#define PTHREAD_ONCE_INIT                                                      \
  { 0 }

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
/** \brief PTHREAD_CANCELED macro. */
#define PTHREAD_CANCELED ((void *)-1)
#ifndef PTHREAD_BARRIER_SERIAL_THREAD
#define PTHREAD_BARRIER_SERIAL_THREAD -1
#endif

/** \brief pthread_atfork function. */
AUTO_WIN_MSVC_EXPORT int pthread_atfork(void (*prepare)(void),
                                        void (*parent)(void),
                                        void (*child)(void));

#if defined(_WIN32)
/** \brief Internal helper to execute prepare handlers */
AUTO_WIN_MSVC_EXPORT void posix_pthread_atfork_prepare(void);
/** \brief Internal helper to execute parent handlers */
AUTO_WIN_MSVC_EXPORT void posix_pthread_atfork_parent(void);
/** \brief Internal helper to execute child handlers */
AUTO_WIN_MSVC_EXPORT void posix_pthread_atfork_child(void);
#endif

/** \brief pthread_attr_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_destroy(pthread_attr_t *attr);
/** \brief pthread_attr_getdetachstate function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getdetachstate(const pthread_attr_t *attr,
                                                     int *detachstate);
/** \brief pthread_attr_getguardsize function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getguardsize(const pthread_attr_t *attr,
                                                   size_t *guardsize);
/** \brief pthread_attr_getinheritsched function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
/** \brief pthread_attr_getschedparam function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getschedparam(const pthread_attr_t *attr,
                                                    struct sched_param *param);
/** \brief pthread_attr_getschedpolicy function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getschedpolicy(const pthread_attr_t *attr,
                                                     int *policy);
/** \brief pthread_attr_getscope function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getscope(const pthread_attr_t *attr,
                                               int *contentionscope);
/** \brief pthread_attr_getstack function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getstack(const pthread_attr_t *attr,
                                               void **stackaddr,
                                               size_t *stacksize);
/** \brief pthread_attr_getstacksize function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_getstacksize(const pthread_attr_t *attr,
                                                   size_t *stacksize);
/** \brief pthread_attr_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_init(pthread_attr_t *attr);
/** \brief pthread_attr_setdetachstate function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setdetachstate(pthread_attr_t *attr,
                                                     int detachstate);
/** \brief pthread_attr_setguardsize function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setguardsize(pthread_attr_t *attr,
                                                   size_t guardsize);
/** \brief pthread_attr_setinheritsched function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setinheritsched(pthread_attr_t *attr,
                                                      int inheritsched);
/** \brief pthread_attr_setschedparam function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT pthread_attr_setschedparam(
    pthread_attr_t *attr, const struct sched_param *param);
/** \brief pthread_attr_setschedpolicy function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setschedpolicy(pthread_attr_t *attr,
                                                     int policy);
/** \brief pthread_attr_setscope function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setscope(pthread_attr_t *attr,
                                               int contentionscope);
/** \brief pthread_attr_setstack function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
/** \brief pthread_attr_setstacksize function. */
AUTO_WIN_MSVC_EXPORT int pthread_attr_setstacksize(pthread_attr_t *attr,
                                                   size_t stacksize);
/** \brief pthread_barrier_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_barrier_destroy(pthread_barrier_t *barrier);
/** \brief pthread_barrier_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_barrier_init(pthread_barrier_t *barrier,
                                              const pthread_barrierattr_t *attr,
                                              unsigned count);
/** \brief pthread_barrier_wait function. */
AUTO_WIN_MSVC_EXPORT int pthread_barrier_wait(pthread_barrier_t *barrier);
/** \brief pthread_barrierattr_destroy function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
/** \brief pthread_barrierattr_getpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
/** \brief pthread_barrierattr_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_barrierattr_init(pthread_barrierattr_t *attr);
/** \brief pthread_barrierattr_setpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
/** \brief pthread_cancel function. */
AUTO_WIN_MSVC_EXPORT int pthread_cancel(pthread_t thread);
struct _pthread_cleanup_buffer {
  void (*routine)(void *);
  void *arg;
  struct _pthread_cleanup_buffer *next;
};

AUTO_WIN_MSVC_EXPORT void AUTO_WIN_MSVC_EXPORT _posix_pthread_cleanup_push(
    struct _pthread_cleanup_buffer *buffer, void (*routine)(void *), void *arg);
AUTO_WIN_MSVC_EXPORT void AUTO_WIN_MSVC_EXPORT
_posix_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);

/** \brief pthread_cleanup_push macro. */
#define pthread_cleanup_push(routine, arg)                                     \
  {                                                                            \
    struct _pthread_cleanup_buffer _cb;                                        \
    _posix_pthread_cleanup_push(&_cb, (routine), (arg));

/** \brief pthread_cleanup_pop macro. */
#define pthread_cleanup_pop(execute)                                           \
  _posix_pthread_cleanup_pop(&_cb, (execute));                                 \
  }
/** \brief pthread_cond_broadcast function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_broadcast(pthread_cond_t *cond);
/** \brief pthread_cond_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_destroy(pthread_cond_t *cond);
/** \brief pthread_cond_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_init(pthread_cond_t *cond,
                                           const pthread_condattr_t *attr);
/** \brief pthread_cond_signal function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_signal(pthread_cond_t *cond);
/** \brief pthread_cond_timedwait function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_timedwait(pthread_cond_t *cond,
                                                pthread_mutex_t *mutex,
                                                const struct timespec *abstime);
/** \brief pthread_cond_wait function. */
AUTO_WIN_MSVC_EXPORT int pthread_cond_wait(pthread_cond_t *cond,
                                           pthread_mutex_t *mutex);
/** \brief pthread_condattr_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_condattr_destroy(pthread_condattr_t *attr);
/** \brief pthread_condattr_getclock function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);
/** \brief pthread_condattr_getpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
/** \brief pthread_condattr_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_condattr_init(pthread_condattr_t *attr);
/** \brief pthread_condattr_setclock function. */
AUTO_WIN_MSVC_EXPORT int pthread_condattr_setclock(pthread_condattr_t *attr,
                                                   clockid_t clock_id);
/** \brief pthread_condattr_setpshared function. */
AUTO_WIN_MSVC_EXPORT int pthread_condattr_setpshared(pthread_condattr_t *attr,
                                                     int pshared);
/** \brief pthread_create function. */
AUTO_WIN_MSVC_EXPORT int pthread_create(pthread_t *thread,
                                        const pthread_attr_t *attr,
                                        void *(*start_routine)(void *),
                                        void *arg);
/** \brief pthread_detach function. */
AUTO_WIN_MSVC_EXPORT int pthread_detach(pthread_t thread);
/** \brief pthread_equal function. */
AUTO_WIN_MSVC_EXPORT int pthread_equal(pthread_t t1, pthread_t t2);
/** \brief pthread_exit function. */
AUTO_WIN_MSVC_EXPORT void pthread_exit(void *value_ptr);
/** \brief pthread_getconcurrency function. */
AUTO_WIN_MSVC_EXPORT int pthread_getconcurrency(void);
/** \brief pthread_getcpuclockid function. */
AUTO_WIN_MSVC_EXPORT int pthread_getcpuclockid(pthread_t thread_id,
                                               clockid_t *clock_id);
/** \brief pthread_getschedparam function. */
AUTO_WIN_MSVC_EXPORT int pthread_getschedparam(pthread_t thread, int *policy,
                                               struct sched_param *param);
AUTO_WIN_MSVC_EXPORT void *pthread_getspecific(pthread_key_t key);
/** \brief pthread_join function. */
AUTO_WIN_MSVC_EXPORT int pthread_join(pthread_t thread, void **value_ptr);
/** \brief pthread_key_create function. */
AUTO_WIN_MSVC_EXPORT int pthread_key_create(pthread_key_t *key,
                                            void (*destructor)(void *));
/** \brief pthread_key_delete function. */
AUTO_WIN_MSVC_EXPORT int pthread_key_delete(pthread_key_t key);
/** \brief pthread_mutex_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutex_destroy(pthread_mutex_t *mutex);
/** \brief pthread_mutex_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutex_init(pthread_mutex_t *mutex,
                                            const pthread_mutexattr_t *attr);
/** \brief pthread_mutex_lock function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutex_lock(pthread_mutex_t *mutex);
/** \brief pthread_mutex_timedlock function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
/** \brief pthread_mutex_trylock function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutex_trylock(pthread_mutex_t *mutex);
/** \brief pthread_mutex_unlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutex_unlock(pthread_mutex_t *mutex);
/** \brief pthread_mutexattr_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
/** \brief pthread_mutexattr_getprioceiling function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT pthread_mutexattr_getprioceiling(
    const pthread_mutexattr_t *attr, int *prioceiling);
/** \brief pthread_mutexattr_getprotocol function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol);
/** \brief pthread_mutexattr_getpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
/** \brief pthread_mutexattr_gettype function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
/** \brief pthread_mutexattr_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutexattr_init(pthread_mutexattr_t *attr);
/** \brief pthread_mutexattr_setprioceiling function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);
/** \brief pthread_mutexattr_setprotocol function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
/** \brief pthread_mutexattr_setpshared function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,
                                                      int pshared);
/** \brief pthread_mutexattr_settype function. */
AUTO_WIN_MSVC_EXPORT int pthread_mutexattr_settype(pthread_mutexattr_t *attr,
                                                   int type);
/** \brief pthread_once function. */
AUTO_WIN_MSVC_EXPORT int pthread_once(pthread_once_t *once_control,
                                      void (*init_routine)(void));
/** \brief pthread_rwlock_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlock_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                                             const pthread_rwlockattr_t *attr);
/** \brief pthread_rwlock_rdlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlock_timedrdlock function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT pthread_rwlock_timedrdlock(
    pthread_rwlock_t *rwlock, const struct timespec *abstime);
/** \brief pthread_rwlock_timedwrlock function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT pthread_rwlock_timedwrlock(
    pthread_rwlock_t *rwlock, const struct timespec *abstime);
/** \brief pthread_rwlock_tryrdlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlock_trywrlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlock_unlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlock_wrlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
/** \brief pthread_rwlockattr_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
/** \brief pthread_rwlockattr_getpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
/** \brief pthread_rwlockattr_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
/** \brief pthread_rwlockattr_setpshared function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);
/** \brief pthread_self function. */
AUTO_WIN_MSVC_EXPORT pthread_t pthread_self(void);
/** \brief pthread_setcancelstate function. */
AUTO_WIN_MSVC_EXPORT int pthread_setcancelstate(int state, int *oldstate);
/** \brief pthread_setcanceltype function. */
AUTO_WIN_MSVC_EXPORT int pthread_setcanceltype(int type, int *oldtype);
/** \brief pthread_setconcurrency function. */
AUTO_WIN_MSVC_EXPORT int pthread_setconcurrency(int new_level);
/** \brief pthread_setschedparam function. */
AUTO_WIN_MSVC_EXPORT int pthread_setschedparam(pthread_t thread, int policy,
                                               const struct sched_param *param);
/** \brief pthread_setschedprio function. */
AUTO_WIN_MSVC_EXPORT int pthread_setschedprio(pthread_t thread, int prio);
/** \brief pthread_setspecific function. */
AUTO_WIN_MSVC_EXPORT int pthread_setspecific(pthread_key_t key,
                                             const void *value);
/** \brief pthread_setname_np function. */
AUTO_WIN_MSVC_EXPORT int pthread_setname_np(pthread_t thread, const char *name);
/** \brief pthread_sigmask function. */
AUTO_WIN_MSVC_EXPORT int pthread_sigmask(int how, const sigset_t *set,
                                         sigset_t *oset);
/** \brief pthread_spin_destroy function. */
AUTO_WIN_MSVC_EXPORT int pthread_spin_destroy(pthread_spinlock_t *lock);
/** \brief pthread_spin_init function. */
AUTO_WIN_MSVC_EXPORT int pthread_spin_init(pthread_spinlock_t *lock,
                                           int pshared);
/** \brief pthread_spin_lock function. */
AUTO_WIN_MSVC_EXPORT int pthread_spin_lock(pthread_spinlock_t *lock);
/** \brief pthread_spin_trylock function. */
AUTO_WIN_MSVC_EXPORT int pthread_spin_trylock(pthread_spinlock_t *lock);
/** \brief pthread_spin_unlock function. */
AUTO_WIN_MSVC_EXPORT int pthread_spin_unlock(pthread_spinlock_t *lock);
/** \brief pthread_testcancel function. */
AUTO_WIN_MSVC_EXPORT void pthread_testcancel(void);
/** \brief sched_get_priority_max function. */
AUTO_WIN_MSVC_EXPORT int sched_get_priority_max(int policy);
/** \brief sched_get_priority_min function. */
AUTO_WIN_MSVC_EXPORT int sched_get_priority_min(int policy);
/** \brief sched_getparam function. */
AUTO_WIN_MSVC_EXPORT int sched_getparam(pid_t pid, struct sched_param *param);
/** \brief sched_getscheduler function. */
AUTO_WIN_MSVC_EXPORT int sched_getscheduler(pid_t pid);
/** \brief sched_rr_get_interval function. */
AUTO_WIN_MSVC_EXPORT int sched_rr_get_interval(pid_t pid,
                                               struct timespec *interval);
/** \brief sched_setparam function. */
AUTO_WIN_MSVC_EXPORT int sched_setparam(pid_t pid,
                                        const struct sched_param *param);
/** \brief sched_setscheduler function. */
AUTO_WIN_MSVC_EXPORT int sched_setscheduler(pid_t pid, int policy,
                                            const struct sched_param *param);
/** \brief sched_yield function. */
AUTO_WIN_MSVC_EXPORT int sched_yield(void);
/** \brief sem_close function. */
AUTO_WIN_MSVC_EXPORT int sem_close(sem_t *sem);
/** \brief sem_destroy function. */
AUTO_WIN_MSVC_EXPORT int sem_destroy(sem_t *sem);
/** \brief sem_getvalue function. */
AUTO_WIN_MSVC_EXPORT int sem_getvalue(sem_t *sem, int *sval);
/** \brief sem_init function. */
AUTO_WIN_MSVC_EXPORT int sem_init(sem_t *sem, int pshared, unsigned int value);
AUTO_WIN_MSVC_EXPORT sem_t *sem_open(const char *name, int oflag, ...);
/** \brief sem_post function. */
AUTO_WIN_MSVC_EXPORT int sem_post(sem_t *sem);
/** \brief sem_timedwait function. */
AUTO_WIN_MSVC_EXPORT int sem_timedwait(sem_t *sem,
                                       const struct timespec *abs_timeout);
/** \brief sem_trywait function. */
AUTO_WIN_MSVC_EXPORT int sem_trywait(sem_t *sem);
/** \brief sem_unlink function. */
AUTO_WIN_MSVC_EXPORT int sem_unlink(const char *name);
/** \brief sem_wait function. */
AUTO_WIN_MSVC_EXPORT int sem_wait(sem_t *sem);

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_PTHREAD_H */

/* posix-pthread.h - Strict C89 Header */
#ifndef POSIX_PTHREAD_H
#define POSIX_PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <time.h>
#if !defined(_WIN32)
#include <sys/types.h>
#include <signal.h>
#endif

#if defined(_WIN32) && !defined(POSIX_PTHREAD_NO_WINDOWS_H)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
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
#endif

#if defined(_WIN32) && !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif

#if defined(_WIN32) && !defined(_SIGSET_T_DEFINED)
#define _SIGSET_T_DEFINED
typedef int sigset_t;
#endif

#if defined(_WIN32) && !defined(_PID_T_DEFINED)
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#if defined(_WIN32) && !defined(_CLOCKID_T_DEFINED)
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

/**
 * @brief pthread_atfork
 * 
 * Polyfill for pthread_atfork.
 */
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
/**
 * @brief pthread_attr_destroy
 * 
 * Polyfill for pthread_attr_destroy.
 */
int pthread_attr_destroy(pthread_attr_t *attr);
/**
 * @brief pthread_attr_getdetachstate
 * 
 * Polyfill for pthread_attr_getdetachstate.
 */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
/**
 * @brief pthread_attr_getguardsize
 * 
 * Polyfill for pthread_attr_getguardsize.
 */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
/**
 * @brief pthread_attr_getinheritsched
 * 
 * Polyfill for pthread_attr_getinheritsched.
 */
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
/**
 * @brief pthread_attr_getschedparam
 * 
 * Polyfill for pthread_attr_getschedparam.
 */
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
/**
 * @brief pthread_attr_getschedpolicy
 * 
 * Polyfill for pthread_attr_getschedpolicy.
 */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
/**
 * @brief pthread_attr_getscope
 * 
 * Polyfill for pthread_attr_getscope.
 */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
/**
 * @brief pthread_attr_getstack
 * 
 * Polyfill for pthread_attr_getstack.
 */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
/**
 * @brief pthread_attr_getstacksize
 * 
 * Polyfill for pthread_attr_getstacksize.
 */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
/**
 * @brief pthread_attr_init
 * 
 * Polyfill for pthread_attr_init.
 */
int pthread_attr_init(pthread_attr_t *attr);
/**
 * @brief pthread_attr_setdetachstate
 * 
 * Polyfill for pthread_attr_setdetachstate.
 */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
/**
 * @brief pthread_attr_setguardsize
 * 
 * Polyfill for pthread_attr_setguardsize.
 */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
/**
 * @brief pthread_attr_setinheritsched
 * 
 * Polyfill for pthread_attr_setinheritsched.
 */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
/**
 * @brief pthread_attr_setschedparam
 * 
 * Polyfill for pthread_attr_setschedparam.
 */
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
/**
 * @brief pthread_attr_setschedpolicy
 * 
 * Polyfill for pthread_attr_setschedpolicy.
 */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
/**
 * @brief pthread_attr_setscope
 * 
 * Polyfill for pthread_attr_setscope.
 */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
/**
 * @brief pthread_attr_setstack
 * 
 * Polyfill for pthread_attr_setstack.
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
/**
 * @brief pthread_attr_setstacksize
 * 
 * Polyfill for pthread_attr_setstacksize.
 */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
/**
 * @brief pthread_barrier_destroy
 * 
 * Polyfill for pthread_barrier_destroy.
 */
int pthread_barrier_destroy(pthread_barrier_t *barrier);
/**
 * @brief pthread_barrier_init
 * 
 * Polyfill for pthread_barrier_init.
 */
int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);
/**
 * @brief pthread_barrier_wait
 * 
 * Polyfill for pthread_barrier_wait.
 */
int pthread_barrier_wait(pthread_barrier_t *barrier);
/**
 * @brief pthread_barrierattr_destroy
 * 
 * Polyfill for pthread_barrierattr_destroy.
 */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
/**
 * @brief pthread_barrierattr_getpshared
 * 
 * Polyfill for pthread_barrierattr_getpshared.
 */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
/**
 * @brief pthread_barrierattr_init
 * 
 * Polyfill for pthread_barrierattr_init.
 */
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
/**
 * @brief pthread_barrierattr_setpshared
 * 
 * Polyfill for pthread_barrierattr_setpshared.
 */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
/**
 * @brief pthread_cancel
 * 
 * Polyfill for pthread_cancel.
 */
int pthread_cancel(pthread_t thread);
/**
 * @brief pthread_cleanup_pop
 * 
 * Polyfill for pthread_cleanup_pop.
 */
void pthread_cleanup_pop(int execute);
/**
 * @brief pthread_cleanup_push
 * 
 * Polyfill for pthread_cleanup_push.
 */
void pthread_cleanup_push(void (*routine)(void *), void *arg);
/**
 * @brief pthread_cond_broadcast
 * 
 * Polyfill for pthread_cond_broadcast.
 */
int pthread_cond_broadcast(pthread_cond_t *cond);
/**
 * @brief pthread_cond_destroy
 * 
 * Polyfill for pthread_cond_destroy.
 */
int pthread_cond_destroy(pthread_cond_t *cond);
/**
 * @brief pthread_cond_init
 * 
 * Polyfill for pthread_cond_init.
 */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
/**
 * @brief pthread_cond_signal
 * 
 * Polyfill for pthread_cond_signal.
 */
int pthread_cond_signal(pthread_cond_t *cond);
/**
 * @brief pthread_cond_timedwait
 * 
 * Polyfill for pthread_cond_timedwait.
 */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
/**
 * @brief pthread_cond_wait
 * 
 * Polyfill for pthread_cond_wait.
 */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
/**
 * @brief pthread_condattr_destroy
 * 
 * Polyfill for pthread_condattr_destroy.
 */
int pthread_condattr_destroy(pthread_condattr_t *attr);
/**
 * @brief pthread_condattr_getclock
 * 
 * Polyfill for pthread_condattr_getclock.
 */
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);
/**
 * @brief pthread_condattr_getpshared
 * 
 * Polyfill for pthread_condattr_getpshared.
 */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
/**
 * @brief pthread_condattr_init
 * 
 * Polyfill for pthread_condattr_init.
 */
int pthread_condattr_init(pthread_condattr_t *attr);
/**
 * @brief pthread_condattr_setclock
 * 
 * Polyfill for pthread_condattr_setclock.
 */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
/**
 * @brief pthread_condattr_setpshared
 * 
 * Polyfill for pthread_condattr_setpshared.
 */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
/**
 * @brief pthread_create
 * 
 * Polyfill for pthread_create.
 */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
/**
 * @brief pthread_detach
 * 
 * Polyfill for pthread_detach.
 */
int pthread_detach(pthread_t thread);
/**
 * @brief pthread_equal
 * 
 * Polyfill for pthread_equal.
 */
int pthread_equal(pthread_t t1, pthread_t t2);
/**
 * @brief pthread_exit
 * 
 * Polyfill for pthread_exit.
 */
void pthread_exit(void *value_ptr);
/**
 * @brief pthread_getconcurrency
 * 
 * Polyfill for pthread_getconcurrency.
 */
int pthread_getconcurrency(void);
/**
 * @brief pthread_getcpuclockid
 * 
 * Polyfill for pthread_getcpuclockid.
 */
int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id);
/**
 * @brief pthread_getschedparam
 * 
 * Polyfill for pthread_getschedparam.
 */
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
/**
 * @brief pthread_getspecific
 * 
 * Polyfill for pthread_getspecific.
 */
void *pthread_getspecific(pthread_key_t key);
/**
 * @brief pthread_join
 * 
 * Polyfill for pthread_join.
 */
int pthread_join(pthread_t thread, void **value_ptr);
/**
 * @brief pthread_key_create
 * 
 * Polyfill for pthread_key_create.
 */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
/**
 * @brief pthread_key_delete
 * 
 * Polyfill for pthread_key_delete.
 */
int pthread_key_delete(pthread_key_t key);
/**
 * @brief pthread_mutex_destroy
 * 
 * Polyfill for pthread_mutex_destroy.
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex);
/**
 * @brief pthread_mutex_init
 * 
 * Polyfill for pthread_mutex_init.
 */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
/**
 * @brief pthread_mutex_lock
 * 
 * Polyfill for pthread_mutex_lock.
 */
int pthread_mutex_lock(pthread_mutex_t *mutex);
/**
 * @brief pthread_mutex_timedlock
 * 
 * Polyfill for pthread_mutex_timedlock.
 */
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
/**
 * @brief pthread_mutex_trylock
 * 
 * Polyfill for pthread_mutex_trylock.
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex);
/**
 * @brief pthread_mutex_unlock
 * 
 * Polyfill for pthread_mutex_unlock.
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex);
/**
 * @brief pthread_mutexattr_destroy
 * 
 * Polyfill for pthread_mutexattr_destroy.
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
/**
 * @brief pthread_mutexattr_getprioceiling
 * 
 * Polyfill for pthread_mutexattr_getprioceiling.
 */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling);
/**
 * @brief pthread_mutexattr_getprotocol
 * 
 * Polyfill for pthread_mutexattr_getprotocol.
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol);
/**
 * @brief pthread_mutexattr_getpshared
 * 
 * Polyfill for pthread_mutexattr_getpshared.
 */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
/**
 * @brief pthread_mutexattr_gettype
 * 
 * Polyfill for pthread_mutexattr_gettype.
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
/**
 * @brief pthread_mutexattr_init
 * 
 * Polyfill for pthread_mutexattr_init.
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
/**
 * @brief pthread_mutexattr_setprioceiling
 * 
 * Polyfill for pthread_mutexattr_setprioceiling.
 */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);
/**
 * @brief pthread_mutexattr_setprotocol
 * 
 * Polyfill for pthread_mutexattr_setprotocol.
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
/**
 * @brief pthread_mutexattr_setpshared
 * 
 * Polyfill for pthread_mutexattr_setpshared.
 */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
/**
 * @brief pthread_mutexattr_settype
 * 
 * Polyfill for pthread_mutexattr_settype.
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
/**
 * @brief pthread_once
 * 
 * Polyfill for pthread_once.
 */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
/**
 * @brief pthread_rwlock_destroy
 * 
 * Polyfill for pthread_rwlock_destroy.
 */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlock_init
 * 
 * Polyfill for pthread_rwlock_init.
 */
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
/**
 * @brief pthread_rwlock_rdlock
 * 
 * Polyfill for pthread_rwlock_rdlock.
 */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlock_timedrdlock
 * 
 * Polyfill for pthread_rwlock_timedrdlock.
 */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
/**
 * @brief pthread_rwlock_timedwrlock
 * 
 * Polyfill for pthread_rwlock_timedwrlock.
 */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
/**
 * @brief pthread_rwlock_tryrdlock
 * 
 * Polyfill for pthread_rwlock_tryrdlock.
 */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlock_trywrlock
 * 
 * Polyfill for pthread_rwlock_trywrlock.
 */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlock_unlock
 * 
 * Polyfill for pthread_rwlock_unlock.
 */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlock_wrlock
 * 
 * Polyfill for pthread_rwlock_wrlock.
 */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
/**
 * @brief pthread_rwlockattr_destroy
 * 
 * Polyfill for pthread_rwlockattr_destroy.
 */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
/**
 * @brief pthread_rwlockattr_getpshared
 * 
 * Polyfill for pthread_rwlockattr_getpshared.
 */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
/**
 * @brief pthread_rwlockattr_init
 * 
 * Polyfill for pthread_rwlockattr_init.
 */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
/**
 * @brief pthread_rwlockattr_setpshared
 * 
 * Polyfill for pthread_rwlockattr_setpshared.
 */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);
/**
 * @brief pthread_self
 * 
 * Polyfill for pthread_self.
 */
pthread_t pthread_self(void);
/**
 * @brief pthread_setcancelstate
 * 
 * Polyfill for pthread_setcancelstate.
 */
int pthread_setcancelstate(int state, int *oldstate);
/**
 * @brief pthread_setcanceltype
 * 
 * Polyfill for pthread_setcanceltype.
 */
int pthread_setcanceltype(int type, int *oldtype);
/**
 * @brief pthread_setconcurrency
 * 
 * Polyfill for pthread_setconcurrency.
 */
int pthread_setconcurrency(int new_level);
/**
 * @brief pthread_setschedparam
 * 
 * Polyfill for pthread_setschedparam.
 */
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
/**
 * @brief pthread_setschedprio
 * 
 * Polyfill for pthread_setschedprio.
 */
int pthread_setschedprio(pthread_t thread, int prio);
/**
 * @brief pthread_setspecific
 * 
 * Polyfill for pthread_setspecific.
 */
int pthread_setspecific(pthread_key_t key, const void *value);
/**
 * @brief pthread_sigmask
 * 
 * Polyfill for pthread_sigmask.
 */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
/**
 * @brief pthread_spin_destroy
 * 
 * Polyfill for pthread_spin_destroy.
 */
int pthread_spin_destroy(pthread_spinlock_t *lock);
/**
 * @brief pthread_spin_init
 * 
 * Polyfill for pthread_spin_init.
 */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
/**
 * @brief pthread_spin_lock
 * 
 * Polyfill for pthread_spin_lock.
 */
int pthread_spin_lock(pthread_spinlock_t *lock);
/**
 * @brief pthread_spin_trylock
 * 
 * Polyfill for pthread_spin_trylock.
 */
int pthread_spin_trylock(pthread_spinlock_t *lock);
/**
 * @brief pthread_spin_unlock
 * 
 * Polyfill for pthread_spin_unlock.
 */
int pthread_spin_unlock(pthread_spinlock_t *lock);
/**
 * @brief pthread_testcancel
 * 
 * Polyfill for pthread_testcancel.
 */
void pthread_testcancel(void);
/**
 * @brief sched_get_priority_max
 * 
 * Polyfill for sched_get_priority_max.
 */
int sched_get_priority_max(int policy);
/**
 * @brief sched_get_priority_min
 * 
 * Polyfill for sched_get_priority_min.
 */
int sched_get_priority_min(int policy);
/**
 * @brief sched_getparam
 * 
 * Polyfill for sched_getparam.
 */
int sched_getparam(pid_t pid, struct sched_param *param);
/**
 * @brief sched_getscheduler
 * 
 * Polyfill for sched_getscheduler.
 */
int sched_getscheduler(pid_t pid);
/**
 * @brief sched_rr_get_interval
 * 
 * Polyfill for sched_rr_get_interval.
 */
int sched_rr_get_interval(pid_t pid, struct timespec *interval);
/**
 * @brief sched_setparam
 * 
 * Polyfill for sched_setparam.
 */
int sched_setparam(pid_t pid, const struct sched_param *param);
/**
 * @brief sched_setscheduler
 * 
 * Polyfill for sched_setscheduler.
 */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
/**
 * @brief sched_yield
 * 
 * Polyfill for sched_yield.
 */
int sched_yield(void);
/**
 * @brief sem_close
 * 
 * Polyfill for sem_close.
 */
int sem_close(sem_t *sem);
/**
 * @brief sem_destroy
 * 
 * Polyfill for sem_destroy.
 */
int sem_destroy(sem_t *sem);
/**
 * @brief sem_getvalue
 * 
 * Polyfill for sem_getvalue.
 */
int sem_getvalue(sem_t *sem, int *sval);
/**
 * @brief sem_init
 * 
 * Polyfill for sem_init.
 */
int sem_init(sem_t *sem, int pshared, unsigned int value);
/**
 * @brief sem_open
 * 
 * Polyfill for sem_open.
 */
sem_t *sem_open(const char *name, int oflag, ...);
/**
 * @brief sem_post
 * 
 * Polyfill for sem_post.
 */
int sem_post(sem_t *sem);
/**
 * @brief sem_timedwait
 * 
 * Polyfill for sem_timedwait.
 */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
/**
 * @brief sem_trywait
 * 
 * Polyfill for sem_trywait.
 */
int sem_trywait(sem_t *sem);
/**
 * @brief sem_unlink
 * 
 * Polyfill for sem_unlink.
 */
int sem_unlink(const char *name);
/**
 * @brief sem_wait
 * 
 * Polyfill for sem_wait.
 */
int sem_wait(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_PTHREAD_H */

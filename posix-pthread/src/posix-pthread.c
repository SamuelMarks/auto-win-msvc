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
    (void)cond;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_destroy */
int pthread_cond_destroy(pthread_cond_t *cond) {
    (void)cond;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_init */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
    (void)cond;
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_signal */
int pthread_cond_signal(pthread_cond_t *cond) {
    (void)cond;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_timedwait */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
    (void)cond;
    (void)mutex;
    (void)abstime;

    return ENOSYS;
}

/* TODO: Implement pthread_cond_wait */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    (void)cond;
    (void)mutex;

    return ENOSYS;
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
    (void)thread;

    return ENOSYS;
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
    (void)thread;
    (void)value_ptr;

    return ENOSYS;
}

/* TODO: Implement pthread_key_create */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
    (void)key;
    (void)destructor;

    return ENOSYS;
}

/* TODO: Implement pthread_key_delete */
int pthread_key_delete(pthread_key_t key) {
    (void)key;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_destroy */
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    (void)mutex;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_init */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    (void)mutex;
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_lock */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
    (void)mutex;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_timedlock */
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    (void)mutex;
    (void)abstime;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_trylock */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    (void)mutex;

    return ENOSYS;
}

/* TODO: Implement pthread_mutex_unlock */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    (void)mutex;

    return ENOSYS;
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
    (void)rwlock;
    (void)attr;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_rdlock */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
    (void)rwlock;

    return ENOSYS;
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
    (void)rwlock;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_trywrlock */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
    (void)rwlock;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_unlock */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
    (void)rwlock;

    return ENOSYS;
}

/* TODO: Implement pthread_rwlock_wrlock */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
    (void)rwlock;

    return ENOSYS;
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
    return 0;
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

/* TODO: Implement pthread_setspecific */
int pthread_setspecific(pthread_key_t key, const void *value) {
    (void)key;
    (void)value;

    return ENOSYS;
}

/* TODO: Implement pthread_sigmask */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset) {
    (void)how;
    (void)set;
    (void)oset;

    return ENOSYS;
}

/* TODO: Implement pthread_spin_destroy */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
    (void)lock;

    return ENOSYS;
}

/* TODO: Implement pthread_spin_init */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
    (void)lock;
    (void)pshared;

    return ENOSYS;
}

/* TODO: Implement pthread_spin_lock */
int pthread_spin_lock(pthread_spinlock_t *lock) {
    (void)lock;

    return ENOSYS;
}

/* TODO: Implement pthread_spin_trylock */
int pthread_spin_trylock(pthread_spinlock_t *lock) {
    (void)lock;

    return ENOSYS;
}

/* TODO: Implement pthread_spin_unlock */
int pthread_spin_unlock(pthread_spinlock_t *lock) {
    (void)lock;

    return ENOSYS;
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
    (void)sem;

    return ENOSYS;
}

/* TODO: Implement sem_getvalue */
int sem_getvalue(sem_t *sem, int *sval) {
    (void)sem;
    (void)sval;

    return ENOSYS;
}

/* TODO: Implement sem_init */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
    (void)sem;
    (void)pshared;
    (void)value;

    return ENOSYS;
}

/* TODO: Implement sem_open */
sem_t *sem_open(const char *name, int oflag, ...) {
    (void)name;
    (void)oflag;

    return 0;
}

/* TODO: Implement sem_post */
int sem_post(sem_t *sem) {
    (void)sem;

    return ENOSYS;
}

/* TODO: Implement sem_timedwait */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
    (void)sem;
    (void)abs_timeout;

    return ENOSYS;
}

/* TODO: Implement sem_trywait */
int sem_trywait(sem_t *sem) {
    (void)sem;

    return ENOSYS;
}

/* TODO: Implement sem_unlink */
int sem_unlink(const char *name) {
    (void)name;

    return ENOSYS;
}

/* TODO: Implement sem_wait */
int sem_wait(sem_t *sem) {
    (void)sem;

    return ENOSYS;
}

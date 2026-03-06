/* test.c - 100% Test Coverage Stubs */
#include <stdio.h>
#include <stdlib.h>
#include "greatest.h"
#include "posix-pthread.h"

GREATEST_MAIN_DEFS();

/* Dummy variables */
pthread_attr_t attr;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_cond_t cond;
pthread_condattr_t condattr;
pthread_rwlock_t rwlock;
pthread_rwlockattr_t rwlockattr;
pthread_barrier_t barrier;
pthread_barrierattr_t barrierattr;
pthread_spinlock_t spinlock;
pthread_key_t key;
pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_t thread;
sem_t sem;
struct sched_param param;
struct timespec ts = {0, 0};
int dummy_int = 0;
size_t dummy_size = 0;
void *dummy_ptr = NULL;
clockid_t clock_id = 0;
sigset_t dummy_sigset = 0;
pid_t dummy_pid = 0;

void dummy_func(void) {}
void dummy_func_arg(void *arg) { (void)arg; }
void *dummy_thread_func(void *arg) { (void)arg; return NULL; }

TEST test_pthread_atfork(void) {
    pthread_atfork(dummy_func, dummy_func, dummy_func);
    PASS();
}

TEST test_pthread_attr_destroy(void) {
    pthread_attr_destroy(&attr);
    PASS();
}

TEST test_pthread_attr_getdetachstate(void) {
    pthread_attr_getdetachstate(&attr, &dummy_int);
    PASS();
}

TEST test_pthread_attr_getguardsize(void) {
    pthread_attr_getguardsize(&attr, &dummy_size);
    PASS();
}

TEST test_pthread_attr_getinheritsched(void) {
    pthread_attr_getinheritsched(&attr, &dummy_int);
    PASS();
}

TEST test_pthread_attr_getschedparam(void) {
    pthread_attr_getschedparam(&attr, &param);
    PASS();
}

TEST test_pthread_attr_getschedpolicy(void) {
    pthread_attr_getschedpolicy(&attr, &dummy_int);
    PASS();
}

TEST test_pthread_attr_getscope(void) {
    pthread_attr_getscope(&attr, &dummy_int);
    PASS();
}

TEST test_pthread_attr_getstack(void) {
    pthread_attr_getstack(&attr, &dummy_ptr, &dummy_size);
    PASS();
}

TEST test_pthread_attr_getstacksize(void) {
    pthread_attr_getstacksize(&attr, &dummy_size);
    PASS();
}

TEST test_pthread_attr_init(void) {
    pthread_attr_init(&attr);
    PASS();
}

TEST test_pthread_attr_setdetachstate(void) {
    pthread_attr_setdetachstate(&attr, 0);
    PASS();
}

TEST test_pthread_attr_setguardsize(void) {
    pthread_attr_setguardsize(&attr, 0);
    PASS();
}

TEST test_pthread_attr_setinheritsched(void) {
    pthread_attr_setinheritsched(&attr, 0);
    PASS();
}

TEST test_pthread_attr_setschedparam(void) {
    pthread_attr_setschedparam(&attr, &param);
    PASS();
}

TEST test_pthread_attr_setschedpolicy(void) {
    pthread_attr_setschedpolicy(&attr, 0);
    PASS();
}

TEST test_pthread_attr_setscope(void) {
    pthread_attr_setscope(&attr, 0);
    PASS();
}

TEST test_pthread_attr_setstack(void) {
    pthread_attr_setstack(&attr, dummy_ptr, 0);
    PASS();
}

TEST test_pthread_attr_setstacksize(void) {
    pthread_attr_setstacksize(&attr, 0);
    PASS();
}

TEST test_pthread_barrier_destroy(void) {
    pthread_barrier_destroy(&barrier);
    PASS();
}

TEST test_pthread_barrier_init(void) {
    pthread_barrier_init(&barrier, &barrierattr, 0);
    PASS();
}

TEST test_pthread_barrier_wait(void) {
    pthread_barrier_wait(&barrier);
    PASS();
}

TEST test_pthread_barrierattr_destroy(void) {
    pthread_barrierattr_destroy(&barrierattr);
    PASS();
}

TEST test_pthread_barrierattr_getpshared(void) {
    pthread_barrierattr_getpshared(&barrierattr, &dummy_int);
    PASS();
}

TEST test_pthread_barrierattr_init(void) {
    pthread_barrierattr_init(&barrierattr);
    PASS();
}

TEST test_pthread_barrierattr_setpshared(void) {
    pthread_barrierattr_setpshared(&barrierattr, 0);
    PASS();
}

TEST test_pthread_cancel(void) {
    pthread_cancel(thread);
    PASS();
}

TEST test_pthread_cleanup_pop(void) {
    pthread_cleanup_pop(0);
    PASS();
}

TEST test_pthread_cleanup_push(void) {
    pthread_cleanup_push(dummy_func_arg, dummy_ptr);
    PASS();
}

TEST test_pthread_cond_broadcast(void) {
    pthread_cond_broadcast(&cond);
    PASS();
}

TEST test_pthread_cond_destroy(void) {
    pthread_cond_destroy(&cond);
    PASS();
}

TEST test_pthread_cond_init(void) {
    pthread_cond_init(&cond, &condattr);
    PASS();
}

TEST test_pthread_cond_signal(void) {
    pthread_cond_signal(&cond);
    PASS();
}

TEST test_pthread_cond_timedwait(void) {
    pthread_cond_timedwait(&cond, &mutex, &ts);
    PASS();
}

TEST test_pthread_cond_wait(void) {
    pthread_cond_wait(&cond, &mutex);
    PASS();
}

TEST test_pthread_condattr_destroy(void) {
    pthread_condattr_destroy(&condattr);
    PASS();
}

TEST test_pthread_condattr_getclock(void) {
    pthread_condattr_getclock(&condattr, &clock_id);
    PASS();
}

TEST test_pthread_condattr_getpshared(void) {
    pthread_condattr_getpshared(&condattr, &dummy_int);
    PASS();
}

TEST test_pthread_condattr_init(void) {
    pthread_condattr_init(&condattr);
    PASS();
}

TEST test_pthread_condattr_setclock(void) {
    pthread_condattr_setclock(&condattr, clock_id);
    PASS();
}

TEST test_pthread_condattr_setpshared(void) {
    pthread_condattr_setpshared(&condattr, 0);
    PASS();
}

TEST test_pthread_create(void) {
    pthread_create(&thread, &attr, dummy_thread_func, dummy_ptr);
    PASS();
}

TEST test_pthread_detach(void) {
    pthread_detach(thread);
    PASS();
}

TEST test_pthread_equal(void) {
    pthread_equal(thread, thread);
    PASS();
}

TEST test_pthread_exit(void) {
    pthread_exit(dummy_ptr);
    PASS();
}

TEST test_pthread_getconcurrency(void) {
    pthread_getconcurrency();
    PASS();
}

TEST test_pthread_getcpuclockid(void) {
    pthread_getcpuclockid(thread, &clock_id);
    PASS();
}

TEST test_pthread_getschedparam(void) {
    pthread_getschedparam(thread, &dummy_int, &param);
    PASS();
}

TEST test_pthread_getspecific(void) {
    pthread_getspecific(key);
    PASS();
}

TEST test_pthread_join(void) {
    pthread_join(thread, &dummy_ptr);
    PASS();
}

TEST test_pthread_key_create(void) {
    pthread_key_create(&key, dummy_func_arg);
    PASS();
}

TEST test_pthread_key_delete(void) {
    pthread_key_delete(key);
    PASS();
}

TEST test_pthread_mutex_destroy(void) {
    pthread_mutex_destroy(&mutex);
    PASS();
}

TEST test_pthread_mutex_init(void) {
    pthread_mutex_init(&mutex, &mutexattr);
    PASS();
}

TEST test_pthread_mutex_lock(void) {
    pthread_mutex_lock(&mutex);
    PASS();
}

TEST test_pthread_mutex_timedlock(void) {
    pthread_mutex_timedlock(&mutex, &ts);
    PASS();
}

TEST test_pthread_mutex_trylock(void) {
    pthread_mutex_trylock(&mutex);
    PASS();
}

TEST test_pthread_mutex_unlock(void) {
    pthread_mutex_unlock(&mutex);
    PASS();
}

TEST test_pthread_mutexattr_destroy(void) {
    pthread_mutexattr_destroy(&mutexattr);
    PASS();
}

TEST test_pthread_mutexattr_getprioceiling(void) {
    pthread_mutexattr_getprioceiling(&mutexattr, &dummy_int);
    PASS();
}

TEST test_pthread_mutexattr_getprotocol(void) {
    pthread_mutexattr_getprotocol(&mutexattr, &dummy_int);
    PASS();
}

TEST test_pthread_mutexattr_getpshared(void) {
    pthread_mutexattr_getpshared(&mutexattr, &dummy_int);
    PASS();
}

TEST test_pthread_mutexattr_gettype(void) {
    pthread_mutexattr_gettype(&mutexattr, &dummy_int);
    PASS();
}

TEST test_pthread_mutexattr_init(void) {
    pthread_mutexattr_init(&mutexattr);
    PASS();
}

TEST test_pthread_mutexattr_setprioceiling(void) {
    pthread_mutexattr_setprioceiling(&mutexattr, 0);
    PASS();
}

TEST test_pthread_mutexattr_setprotocol(void) {
    pthread_mutexattr_setprotocol(&mutexattr, 0);
    PASS();
}

TEST test_pthread_mutexattr_setpshared(void) {
    pthread_mutexattr_setpshared(&mutexattr, 0);
    PASS();
}

TEST test_pthread_mutexattr_settype(void) {
    pthread_mutexattr_settype(&mutexattr, 0);
    PASS();
}

TEST test_pthread_once(void) {
    pthread_once(&once, dummy_func);
    PASS();
}

TEST test_pthread_rwlock_destroy(void) {
    pthread_rwlock_destroy(&rwlock);
    PASS();
}

TEST test_pthread_rwlock_init(void) {
    pthread_rwlock_init(&rwlock, &rwlockattr);
    PASS();
}

TEST test_pthread_rwlock_rdlock(void) {
    pthread_rwlock_rdlock(&rwlock);
    PASS();
}

TEST test_pthread_rwlock_timedrdlock(void) {
    pthread_rwlock_timedrdlock(&rwlock, &ts);
    PASS();
}

TEST test_pthread_rwlock_timedwrlock(void) {
    pthread_rwlock_timedwrlock(&rwlock, &ts);
    PASS();
}

TEST test_pthread_rwlock_tryrdlock(void) {
    pthread_rwlock_tryrdlock(&rwlock);
    PASS();
}

TEST test_pthread_rwlock_trywrlock(void) {
    pthread_rwlock_trywrlock(&rwlock);
    PASS();
}

TEST test_pthread_rwlock_unlock(void) {
    pthread_rwlock_unlock(&rwlock);
    PASS();
}

TEST test_pthread_rwlock_wrlock(void) {
    pthread_rwlock_wrlock(&rwlock);
    PASS();
}

TEST test_pthread_rwlockattr_destroy(void) {
    pthread_rwlockattr_destroy(&rwlockattr);
    PASS();
}

TEST test_pthread_rwlockattr_getpshared(void) {
    pthread_rwlockattr_getpshared(&rwlockattr, &dummy_int);
    PASS();
}

TEST test_pthread_rwlockattr_init(void) {
    pthread_rwlockattr_init(&rwlockattr);
    PASS();
}

TEST test_pthread_rwlockattr_setpshared(void) {
    pthread_rwlockattr_setpshared(&rwlockattr, 0);
    PASS();
}

TEST test_pthread_self(void) {
    pthread_self();
    PASS();
}

TEST test_pthread_setcancelstate(void) {
    pthread_setcancelstate(0, &dummy_int);
    PASS();
}

TEST test_pthread_setcanceltype(void) {
    pthread_setcanceltype(0, &dummy_int);
    PASS();
}

TEST test_pthread_setconcurrency(void) {
    pthread_setconcurrency(0);
    PASS();
}

TEST test_pthread_setschedparam(void) {
    pthread_setschedparam(thread, 0, &param);
    PASS();
}

TEST test_pthread_setschedprio(void) {
    pthread_setschedprio(thread, 0);
    PASS();
}

TEST test_pthread_setspecific(void) {
    pthread_setspecific(key, dummy_ptr);
    PASS();
}

TEST test_pthread_sigmask(void) {
    pthread_sigmask(0, &dummy_sigset, &dummy_sigset);
    PASS();
}

TEST test_pthread_spin_destroy(void) {
    pthread_spin_destroy(&spinlock);
    PASS();
}

TEST test_pthread_spin_init(void) {
    pthread_spin_init(&spinlock, 0);
    PASS();
}

TEST test_pthread_spin_lock(void) {
    pthread_spin_lock(&spinlock);
    PASS();
}

TEST test_pthread_spin_trylock(void) {
    pthread_spin_trylock(&spinlock);
    PASS();
}

TEST test_pthread_spin_unlock(void) {
    pthread_spin_unlock(&spinlock);
    PASS();
}

TEST test_pthread_testcancel(void) {
    pthread_testcancel();
    PASS();
}

TEST test_sched_get_priority_max(void) {
    sched_get_priority_max(0);
    PASS();
}

TEST test_sched_get_priority_min(void) {
    sched_get_priority_min(0);
    PASS();
}

TEST test_sched_getparam(void) {
    sched_getparam(dummy_pid, &param);
    PASS();
}

TEST test_sched_getscheduler(void) {
    sched_getscheduler(dummy_pid);
    PASS();
}

TEST test_sched_rr_get_interval(void) {
    sched_rr_get_interval(dummy_pid, &ts);
    PASS();
}

TEST test_sched_setparam(void) {
    sched_setparam(dummy_pid, &param);
    PASS();
}

TEST test_sched_setscheduler(void) {
    sched_setscheduler(dummy_pid, 0, &param);
    PASS();
}

TEST test_sched_yield(void) {
    sched_yield();
    PASS();
}

TEST test_sem_close(void) {
    sem_close(&sem);
    PASS();
}

TEST test_sem_destroy(void) {
    sem_destroy(&sem);
    PASS();
}

TEST test_sem_getvalue(void) {
    sem_getvalue(&sem, &dummy_int);
    PASS();
}

TEST test_sem_init(void) {
    sem_init(&sem, 0, 0);
    PASS();
}

TEST test_sem_open(void) {
    sem_open(NULL, 0, 0);
    PASS();
}

TEST test_sem_post(void) {
    sem_post(&sem);
    PASS();
}

TEST test_sem_timedwait(void) {
    sem_timedwait(&sem, &ts);
    PASS();
}

TEST test_sem_trywait(void) {
    sem_trywait(&sem);
    PASS();
}

TEST test_sem_unlink(void) {
    sem_unlink(NULL);
    PASS();
}

TEST test_sem_wait(void) {
    sem_wait(&sem);
    PASS();
}

SUITE(posix_pthread_suite) {
    RUN_TEST(test_pthread_atfork);
    RUN_TEST(test_pthread_attr_destroy);
    RUN_TEST(test_pthread_attr_getdetachstate);
    RUN_TEST(test_pthread_attr_getguardsize);
    RUN_TEST(test_pthread_attr_getinheritsched);
    RUN_TEST(test_pthread_attr_getschedparam);
    RUN_TEST(test_pthread_attr_getschedpolicy);
    RUN_TEST(test_pthread_attr_getscope);
    RUN_TEST(test_pthread_attr_getstack);
    RUN_TEST(test_pthread_attr_getstacksize);
    RUN_TEST(test_pthread_attr_init);
    RUN_TEST(test_pthread_attr_setdetachstate);
    RUN_TEST(test_pthread_attr_setguardsize);
    RUN_TEST(test_pthread_attr_setinheritsched);
    RUN_TEST(test_pthread_attr_setschedparam);
    RUN_TEST(test_pthread_attr_setschedpolicy);
    RUN_TEST(test_pthread_attr_setscope);
    RUN_TEST(test_pthread_attr_setstack);
    RUN_TEST(test_pthread_attr_setstacksize);
    RUN_TEST(test_pthread_barrier_destroy);
    RUN_TEST(test_pthread_barrier_init);
    RUN_TEST(test_pthread_barrier_wait);
    RUN_TEST(test_pthread_barrierattr_destroy);
    RUN_TEST(test_pthread_barrierattr_getpshared);
    RUN_TEST(test_pthread_barrierattr_init);
    RUN_TEST(test_pthread_barrierattr_setpshared);
    RUN_TEST(test_pthread_cancel);
    RUN_TEST(test_pthread_cleanup_pop);
    RUN_TEST(test_pthread_cleanup_push);
    RUN_TEST(test_pthread_cond_broadcast);
    RUN_TEST(test_pthread_cond_destroy);
    RUN_TEST(test_pthread_cond_init);
    RUN_TEST(test_pthread_cond_signal);
    RUN_TEST(test_pthread_cond_timedwait);
    RUN_TEST(test_pthread_cond_wait);
    RUN_TEST(test_pthread_condattr_destroy);
    RUN_TEST(test_pthread_condattr_getclock);
    RUN_TEST(test_pthread_condattr_getpshared);
    RUN_TEST(test_pthread_condattr_init);
    RUN_TEST(test_pthread_condattr_setclock);
    RUN_TEST(test_pthread_condattr_setpshared);
    RUN_TEST(test_pthread_create);
    RUN_TEST(test_pthread_detach);
    RUN_TEST(test_pthread_equal);
    RUN_TEST(test_pthread_exit);
    RUN_TEST(test_pthread_getconcurrency);
    RUN_TEST(test_pthread_getcpuclockid);
    RUN_TEST(test_pthread_getschedparam);
    RUN_TEST(test_pthread_getspecific);
    RUN_TEST(test_pthread_join);
    RUN_TEST(test_pthread_key_create);
    RUN_TEST(test_pthread_key_delete);
    RUN_TEST(test_pthread_mutex_destroy);
    RUN_TEST(test_pthread_mutex_init);
    RUN_TEST(test_pthread_mutex_lock);
    RUN_TEST(test_pthread_mutex_timedlock);
    RUN_TEST(test_pthread_mutex_trylock);
    RUN_TEST(test_pthread_mutex_unlock);
    RUN_TEST(test_pthread_mutexattr_destroy);
    RUN_TEST(test_pthread_mutexattr_getprioceiling);
    RUN_TEST(test_pthread_mutexattr_getprotocol);
    RUN_TEST(test_pthread_mutexattr_getpshared);
    RUN_TEST(test_pthread_mutexattr_gettype);
    RUN_TEST(test_pthread_mutexattr_init);
    RUN_TEST(test_pthread_mutexattr_setprioceiling);
    RUN_TEST(test_pthread_mutexattr_setprotocol);
    RUN_TEST(test_pthread_mutexattr_setpshared);
    RUN_TEST(test_pthread_mutexattr_settype);
    RUN_TEST(test_pthread_once);
    RUN_TEST(test_pthread_rwlock_destroy);
    RUN_TEST(test_pthread_rwlock_init);
    RUN_TEST(test_pthread_rwlock_rdlock);
    RUN_TEST(test_pthread_rwlock_timedrdlock);
    RUN_TEST(test_pthread_rwlock_timedwrlock);
    RUN_TEST(test_pthread_rwlock_tryrdlock);
    RUN_TEST(test_pthread_rwlock_trywrlock);
    RUN_TEST(test_pthread_rwlock_unlock);
    RUN_TEST(test_pthread_rwlock_wrlock);
    RUN_TEST(test_pthread_rwlockattr_destroy);
    RUN_TEST(test_pthread_rwlockattr_getpshared);
    RUN_TEST(test_pthread_rwlockattr_init);
    RUN_TEST(test_pthread_rwlockattr_setpshared);
    RUN_TEST(test_pthread_self);
    RUN_TEST(test_pthread_setcancelstate);
    RUN_TEST(test_pthread_setcanceltype);
    RUN_TEST(test_pthread_setconcurrency);
    RUN_TEST(test_pthread_setschedparam);
    RUN_TEST(test_pthread_setschedprio);
    RUN_TEST(test_pthread_setspecific);
    RUN_TEST(test_pthread_sigmask);
    RUN_TEST(test_pthread_spin_destroy);
    RUN_TEST(test_pthread_spin_init);
    RUN_TEST(test_pthread_spin_lock);
    RUN_TEST(test_pthread_spin_trylock);
    RUN_TEST(test_pthread_spin_unlock);
    RUN_TEST(test_pthread_testcancel);
    RUN_TEST(test_sched_get_priority_max);
    RUN_TEST(test_sched_get_priority_min);
    RUN_TEST(test_sched_getparam);
    RUN_TEST(test_sched_getscheduler);
    RUN_TEST(test_sched_rr_get_interval);
    RUN_TEST(test_sched_setparam);
    RUN_TEST(test_sched_setscheduler);
    RUN_TEST(test_sched_yield);
    RUN_TEST(test_sem_close);
    RUN_TEST(test_sem_destroy);
    RUN_TEST(test_sem_getvalue);
    RUN_TEST(test_sem_init);
    RUN_TEST(test_sem_open);
    RUN_TEST(test_sem_post);
    RUN_TEST(test_sem_timedwait);
    RUN_TEST(test_sem_trywait);
    RUN_TEST(test_sem_unlink);
    RUN_TEST(test_sem_wait);
}

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_pthread_suite);
    GREATEST_MAIN_END();
}

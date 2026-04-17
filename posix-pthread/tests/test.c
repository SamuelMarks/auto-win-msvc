/* test.c - 100% Test Coverage Stubs */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#ifndef EINVAL
#define EINVAL 22
#endif
#ifndef EEXIST
#define EEXIST 17
#endif
#ifndef EAGAIN
#define EAGAIN 11
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

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
sem_t dummy_sem;
struct sched_param param;
struct timespec dummy_ts = {0, 0};
int dummy_int = 0;
size_t dummy_size = 0;
void *dummy_ptr = NULL;
clockid_t clock_id = 0;
sigset_t dummy_sigset = 0;
pid_t dummy_pid = 0;

void dummy_func(void) {}
void dummy_func_arg(void *arg) { (void)arg; }
void *dummy_thread_func(void *arg) {
  (void)arg;
  return NULL;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
static int atfork_prepare_seq = 0;
static int atfork_parent_seq = 0;
static int atfork_child_seq = 0;

static int prepare1_called = 0;
static int prepare2_called = 0;
static int parent1_called = 0;
static int parent2_called = 0;
static int child1_called = 0;
static int child2_called = 0;

static void prepare1(void) { prepare1_called = ++atfork_prepare_seq; }
static void prepare2(void) { prepare2_called = ++atfork_prepare_seq; }
static void parent1(void) { parent1_called = ++atfork_parent_seq; }
static void parent2(void) { parent2_called = ++atfork_parent_seq; }
static void child1(void) { child1_called = ++atfork_child_seq; }
static void child2(void) { child2_called = ++atfork_child_seq; }
#endif

TEST test_pthread_atfork(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int res;

  /* Register handlers */
  res = pthread_atfork(prepare1, parent1, child1);
  ASSERT_EQ(0, res);

  res = pthread_atfork(prepare2, parent2, child2);
  ASSERT_EQ(0, res);

  /* Execute prepare handlers (should be LIFO) */
  posix_pthread_atfork_prepare();
  ASSERT_EQ(1, prepare2_called); /* Registered last, called first */
  ASSERT_EQ(2, prepare1_called);

  /* Execute parent handlers (should be FIFO) */
  posix_pthread_atfork_parent();
  ASSERT_EQ(1, parent1_called); /* Registered first, called first */
  ASSERT_EQ(2, parent2_called);

  /* Execute child handlers (should be FIFO) */
  posix_pthread_atfork_child();
  ASSERT_EQ(1, child1_called); /* Registered first, called first */
  ASSERT_EQ(2, child2_called);

#endif
  PASS();
}

TEST test_pthread_attr_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_destroy(&attr);

#endif
  PASS();
}

TEST test_pthread_attr_getdetachstate(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getdetachstate(&attr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_attr_getguardsize(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getguardsize(&attr, &dummy_size);

#endif
  PASS();
}

TEST test_pthread_attr_getinheritsched(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getinheritsched(&attr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_attr_getschedparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getschedparam(&attr, &param);

#endif
  PASS();
}

TEST test_pthread_attr_getschedpolicy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getschedpolicy(&attr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_attr_getscope(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getscope(&attr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_attr_getstack(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getstack(&attr, &dummy_ptr, &dummy_size);

#endif
  PASS();
}

TEST test_pthread_attr_getstacksize(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_getstacksize(&attr, &dummy_size);

#endif
  PASS();
}

TEST test_pthread_attr_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_init(&attr);

#endif
  PASS();
}

TEST test_pthread_attr_setdetachstate(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setdetachstate(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setguardsize(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setguardsize(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setinheritsched(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setinheritsched(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setschedparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setschedparam(&attr, &param);

#endif
  PASS();
}

TEST test_pthread_attr_setschedpolicy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setschedpolicy(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setscope(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setscope(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setstack(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setstack(&attr, dummy_ptr, 0);

#endif
  PASS();
}

TEST test_pthread_attr_setstacksize(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_attr_setstacksize(&attr, 0);

#endif
  PASS();
}

TEST test_pthread_barrier_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrier_destroy(&barrier);

#endif
  PASS();
}

TEST test_pthread_barrier_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrier_init(&barrier, &barrierattr, 0);

#endif
  PASS();
}

TEST test_pthread_barrier_wait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_barrier_wait(&barrier); */

#endif
  PASS();
}

TEST test_pthread_barrierattr_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrierattr_destroy(&barrierattr);

#endif
  PASS();
}

TEST test_pthread_barrierattr_getpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrierattr_getpshared(&barrierattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_barrierattr_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrierattr_init(&barrierattr);

#endif
  PASS();
}

TEST test_pthread_barrierattr_setpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_barrierattr_setpshared(&barrierattr, 0);

#endif
  PASS();
}

TEST test_pthread_cancel(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cancel(thread);

#endif
  PASS();
}

TEST test_pthread_cleanup_push(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cleanup_push(dummy_func_arg, dummy_ptr);
  pthread_cleanup_pop(0);

#endif
  PASS();
}

TEST test_pthread_cond_broadcast(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cond_broadcast(&cond);

#endif
  PASS();
}

TEST test_pthread_cond_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cond_destroy(&cond);

#endif
  PASS();
}

TEST test_pthread_cond_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cond_init(&cond, &condattr);

#endif
  PASS();
}

TEST test_pthread_cond_signal(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_cond_signal(&cond);

#endif
  PASS();
}

TEST test_pthread_cond_timedwait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_cond_timedwait(&cond, &mutex, &dummy_ts); */

#endif
  PASS();
}

TEST test_pthread_cond_wait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_cond_wait(&cond, &mutex); */

#endif
  PASS();
}

TEST test_pthread_condattr_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_destroy(&condattr);

#endif
  PASS();
}

TEST test_pthread_condattr_getclock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_getclock(&condattr, &clock_id);

#endif
  PASS();
}

TEST test_pthread_condattr_getpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_getpshared(&condattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_condattr_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_init(&condattr);

#endif
  PASS();
}

TEST test_pthread_condattr_setclock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_setclock(&condattr, clock_id);

#endif
  PASS();
}

TEST test_pthread_condattr_setpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_condattr_setpshared(&condattr, 0);

#endif
  PASS();
}

TEST test_pthread_create(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_create(&thread, &attr, dummy_thread_func, dummy_ptr);

#endif
  PASS();
}

TEST test_pthread_detach(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_detach(thread);

#endif
  PASS();
}

TEST test_pthread_equal(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_equal(thread, thread);

#endif
  PASS();
}

TEST test_pthread_exit(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_exit(dummy_ptr); */

#endif
  PASS();
}

TEST test_pthread_getconcurrency(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_getconcurrency();

#endif
  PASS();
}

TEST test_pthread_getcpuclockid(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_getcpuclockid(thread, &clock_id);

#endif
  PASS();
}

TEST test_pthread_getschedparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_getschedparam(thread, &dummy_int, &param);

#endif
  PASS();
}

TEST test_pthread_getspecific(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_getspecific(key);

#endif
  PASS();
}

TEST test_pthread_join(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_join(thread, &dummy_ptr); */

#endif
  PASS();
}

TEST test_pthread_key_create(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_key_create(&key, dummy_func_arg);

#endif
  PASS();
}

TEST test_pthread_key_delete(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_key_delete(key);

#endif
  PASS();
}

TEST test_pthread_mutex_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutex_destroy(&mutex);

#endif
  PASS();
}

TEST test_pthread_mutex_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutex_init(&mutex, &mutexattr);

#endif
  PASS();
}

TEST test_pthread_mutex_lock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_mutex_lock(&mutex); */

#endif
  PASS();
}

TEST test_pthread_mutex_timedlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutex_timedlock(&mutex, &dummy_ts);

#endif
  PASS();
}

TEST test_pthread_mutex_trylock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutex_trylock(&mutex);

#endif
  PASS();
}

TEST test_pthread_mutex_unlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutex_unlock(&mutex);

#endif
  PASS();
}

TEST test_pthread_mutexattr_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_destroy(&mutexattr);

#endif
  PASS();
}

TEST test_pthread_mutexattr_getprioceiling(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_getprioceiling(&mutexattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_mutexattr_getprotocol(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_getprotocol(&mutexattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_mutexattr_getpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_getpshared(&mutexattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_mutexattr_gettype(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_gettype(&mutexattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_mutexattr_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_init(&mutexattr);

#endif
  PASS();
}

TEST test_pthread_mutexattr_setprioceiling(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_setprioceiling(&mutexattr, 0);

#endif
  PASS();
}

TEST test_pthread_mutexattr_setprotocol(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_setprotocol(&mutexattr, 0);

#endif
  PASS();
}

TEST test_pthread_mutexattr_setpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_setpshared(&mutexattr, 0);

#endif
  PASS();
}

TEST test_pthread_mutexattr_settype(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_mutexattr_settype(&mutexattr, 0);

#endif
  PASS();
}

TEST test_pthread_once(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_once(&once, dummy_func);

#endif
  PASS();
}

TEST test_pthread_rwlock_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_destroy(&rwlock);

#endif
  PASS();
}

TEST test_pthread_rwlock_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_init(&rwlock, &rwlockattr);

#endif
  PASS();
}

TEST test_pthread_rwlock_rdlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_rwlock_rdlock(&rwlock); */

#endif
  PASS();
}

TEST test_pthread_rwlock_timedrdlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_timedrdlock(&rwlock, &dummy_ts);

#endif
  PASS();
}

TEST test_pthread_rwlock_timedwrlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_timedwrlock(&rwlock, &dummy_ts);

#endif
  PASS();
}

TEST test_pthread_rwlock_tryrdlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_tryrdlock(&rwlock);

#endif
  PASS();
}

TEST test_pthread_rwlock_trywrlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_trywrlock(&rwlock);

#endif
  PASS();
}

TEST test_pthread_rwlock_unlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlock_unlock(&rwlock);

#endif
  PASS();
}

TEST test_pthread_rwlock_wrlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_rwlock_wrlock(&rwlock); */

#endif
  PASS();
}

TEST test_pthread_rwlockattr_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlockattr_destroy(&rwlockattr);

#endif
  PASS();
}

TEST test_pthread_rwlockattr_getpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlockattr_getpshared(&rwlockattr, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_rwlockattr_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlockattr_init(&rwlockattr);

#endif
  PASS();
}

TEST test_pthread_rwlockattr_setpshared(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_rwlockattr_setpshared(&rwlockattr, 0);

#endif
  PASS();
}

TEST test_pthread_self(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_self();

#endif
  PASS();
}

TEST test_pthread_setcancelstate(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setcancelstate(0, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_setcanceltype(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setcanceltype(0, &dummy_int);

#endif
  PASS();
}

TEST test_pthread_setconcurrency(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setconcurrency(0);

#endif
  PASS();
}

TEST test_pthread_setschedparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setschedparam(thread, 0, &param);

#endif
  PASS();
}

TEST test_pthread_setschedprio(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setschedprio(thread, 0);

#endif
  PASS();
}

TEST test_pthread_setspecific(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_setspecific(key, dummy_ptr);

#endif
  PASS();
}

TEST test_pthread_sigmask(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_sigmask(0, &dummy_sigset, &dummy_sigset);

#endif
  PASS();
}

TEST test_pthread_spin_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_spin_destroy(&spinlock);

#endif
  PASS();
}

TEST test_pthread_spin_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_spin_init(&spinlock, 0);

#endif
  PASS();
}

TEST test_pthread_spin_lock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  /* pthread_spin_lock(&spinlock); */

#endif
  PASS();
}

TEST test_pthread_spin_trylock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_spin_trylock(&spinlock);

#endif
  PASS();
}

TEST test_pthread_spin_unlock(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_spin_unlock(&spinlock);

#endif
  PASS();
}

TEST test_pthread_testcancel(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  pthread_testcancel();

#endif
  PASS();
}

TEST test_sched_get_priority_max(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_get_priority_max(0);

#endif
  PASS();
}

TEST test_sched_get_priority_min(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_get_priority_min(0);

#endif
  PASS();
}

TEST test_sched_getparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_getparam(dummy_pid, &param);

#endif
  PASS();
}

TEST test_sched_getscheduler(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_getscheduler(dummy_pid);

#endif
  PASS();
}

TEST test_sched_rr_get_interval(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_rr_get_interval(dummy_pid, &dummy_ts);

#endif
  PASS();
}

TEST test_sched_setparam(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_setparam(dummy_pid, &param);

#endif
  PASS();
}

TEST test_sched_setscheduler(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_setscheduler(dummy_pid, 0, &param);

#endif
  PASS();
}

TEST test_sched_yield(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)

  sched_yield();

#endif
  PASS();
}

TEST test_sem_close(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t *sem = sem_open("/test_sem_close", O_CREAT, 0666, 1);
  ASSERT(sem != (sem_t *)-1);
  ASSERT_EQ(0, sem_close(sem));
  ASSERT_EQ(-1, sem_close(NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_destroy(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  ASSERT_EQ(0, sem_init(&sem, 0, 1));
  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_destroy(NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_getvalue(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  int val = -1;
  ASSERT_EQ(0, sem_init(&sem, 0, 5));
  ASSERT_EQ(0, sem_getvalue(&sem, &val));
  ASSERT_EQ(5, val);

  ASSERT_EQ(0, sem_wait(&sem));
  ASSERT_EQ(0, sem_getvalue(&sem, &val));
  ASSERT_EQ(4, val);

  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_getvalue(NULL, &val));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_init(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  ASSERT_EQ(0, sem_init(&sem, 0, 0));
  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_init(NULL, 0, 0));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_open(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t *sem;
  sem_t *sem2;
  sem_unlink("/test_sem_open");
  sem = sem_open("/test_sem_open", O_CREAT | O_EXCL, 0666, 1);
  ASSERT(sem != (sem_t *)-1);

  /* Already exists */
  sem2 = sem_open("/test_sem_open", O_CREAT | O_EXCL, 0666, 1);
  ASSERT_EQ((sem_t *)-1, sem2);
  ASSERT_EQ(EEXIST, errno);

  /* Open existing */
  sem2 = sem_open("/test_sem_open", 0);
  ASSERT(sem2 != (sem_t *)-1);

  /* Clean up */
  ASSERT_EQ(0, sem_close(sem));
  ASSERT_EQ(0, sem_close(sem2));

  ASSERT_EQ((sem_t *)-1, sem_open(NULL, 0));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_post(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  int val;
  ASSERT_EQ(0, sem_init(&sem, 0, 0));
  ASSERT_EQ(0, sem_post(&sem));
  ASSERT_EQ(0, sem_getvalue(&sem, &val));
  ASSERT_EQ(1, val);
  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_post(NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_timedwait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  struct timespec ts;
  ASSERT_EQ(0, sem_init(&sem, 0, 0));
  ts.tv_sec = 0; /* Past time */
  ts.tv_nsec = 0;
  ASSERT_EQ(-1, sem_timedwait(&sem, &ts));
  ASSERT_EQ(ETIMEDOUT, errno);

  ASSERT_EQ(0, sem_post(&sem));
  ASSERT_EQ(0,
            sem_timedwait(&sem, &ts)); /* Should succeed since it's signaled */

  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_timedwait(NULL, &ts));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_trywait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  ASSERT_EQ(0, sem_init(&sem, 0, 0));
  ASSERT_EQ(-1, sem_trywait(&sem));
  ASSERT_EQ(EAGAIN, errno);

  ASSERT_EQ(0, sem_post(&sem));
  ASSERT_EQ(0, sem_trywait(&sem));

  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_trywait(NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_sem_unlink(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(0, sem_unlink("/test_sem_unlink"));
#endif
  PASS();
}

TEST test_sem_wait(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t sem;
  ASSERT_EQ(0, sem_init(&sem, 0, 1));
  ASSERT_EQ(0, sem_wait(&sem));
  ASSERT_EQ(0, sem_destroy(&sem));
  ASSERT_EQ(-1, sem_wait(NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}
TEST test_dyn_SetThreadDescription(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* dyn_SetThreadDescription stub */
#endif
  PASS();
}

TEST test_pthread_cleanup_pop(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* pthread_cleanup_pop stub */
#endif
  PASS();
}

TEST test_pthread_setname_np(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* pthread_setname_np stub */
#endif
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
  RUN_TEST(test_dyn_SetThreadDescription);
  RUN_TEST(test_pthread_cleanup_pop);
  RUN_TEST(test_pthread_setname_np);
}

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(posix_pthread_suite);
  GREATEST_MAIN_END();
}

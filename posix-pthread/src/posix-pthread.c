/* posix-pthread.c - Strict C89 Implementation */
#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__) ||          \
    defined(__EMSCRIPTEN__)

/**
 * @file posix-pthread.c
 * @brief Portable POSIX threads, barriers, spinlocks, scheduling, and
 * semaphores polyfill for Windows and DOS targets.
 */

/* clang-format off */
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__DOS__) ||           \
    defined(__MSDOS__) || defined(__WATCOMC__) || defined(__EMSCRIPTEN__)
#include <fcntl.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#endif
/* clang-format on */

#ifndef EINVAL
#define EINVAL 22
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif
#ifndef EBUSY
#define EBUSY 16
#endif
#ifndef EAGAIN
#define EAGAIN 11
#endif
#ifndef ENOMEM
#define ENOMEM 12
#endif
#ifndef ENOENT
#define ENOENT 2
#endif
#ifndef EEXIST
#define EEXIST 17
#endif
#ifndef ENOSYS
#define ENOSYS 40
#endif

#if (!defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&           \
     !defined(__WATCOMC__)) ||                                                 \
    defined(__CYGWIN__)
/* Native POSIX threads used on Unix and Cygwin */
#if defined(__EMSCRIPTEN__)
/** @brief Set thread scheduling policy and parameters on Emscripten. */
int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *param) {
  (void)thread;
  if (!param ||
      (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR)) {
    return EINVAL;
  }
  return 0;
}

/** @brief Set thread debugging name on Emscripten. */
int pthread_setname_np(pthread_t thread, const char *name) {
  if (!thread || !name) {
    return EINVAL;
  }
  return 0;
}

#if !defined(__EMSCRIPTEN_PTHREADS__)
/** @brief Get maximum priority for scheduling policy on Emscripten. */
int sched_get_priority_max(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return (policy == SCHED_OTHER) ? 0 : 15;
}

/** @brief Get minimum priority for scheduling policy on Emscripten. */
int sched_get_priority_min(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return (policy == SCHED_OTHER) ? 0 : 1;
}
#endif /* !__EMSCRIPTEN_PTHREADS__ */

/** @brief Get process scheduling parameters on Emscripten. */
int sched_getparam(pid_t pid, struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = (pid < 0) ? ESRCH : EINVAL;
    return -1;
  }
  param->sched_priority = 0;
  return 0;
}

/** @brief Get process scheduling policy on Emscripten. */
int sched_getscheduler(pid_t pid) {
  if (pid < 0) {
    errno = ESRCH;
    return -1;
  }
  return SCHED_OTHER;
}

/** @brief Get round-robin time quantum on Emscripten. */
int sched_rr_get_interval(pid_t pid, struct timespec *interval) {
  if (pid < 0 || !interval) {
    errno = (pid < 0) ? ESRCH : EINVAL;
    return -1;
  }
  interval->tv_sec = 0;
  interval->tv_nsec = 15000000L;
  return 0;
}

/** @brief Set process scheduling parameters on Emscripten. */
int sched_setparam(pid_t pid, const struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = (pid < 0) ? ESRCH : EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set process scheduling policy and parameters on Emscripten. */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
  if (pid < 0 || !param ||
      (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR)) {
    errno = (pid < 0) ? ESRCH : EINVAL;
    return -1;
  }
  return 0;
}

#if !defined(__EMSCRIPTEN_PTHREADS__)
/** @brief Query semaphore count on Emscripten. */
int sem_getvalue(sem_t *sem, int *sval) {
  int count = 0;
  int i;
  if (!sem || !sval) {
    errno = EINVAL;
    return -1;
  }
  while (count < 32767 && sem_trywait(sem) == 0) {
    count++;
  }
  for (i = 0; i < count; ++i) {
    (void)sem_post(sem);
  }
  *sval = count;
  return 0;
}

/** @brief Decrement semaphore with timeout on Emscripten. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  if (!sem || !abs_timeout || abs_timeout->tv_nsec < 0L ||
      abs_timeout->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  if (sem_trywait(sem) == 0) {
    return 0;
  }
  errno = ETIMEDOUT;
  return -1;
}

/** @brief Open or create a named semaphore on Emscripten. */
sem_t *sem_open(const char *name, int oflag, ...) {
  sem_t *s;
  unsigned int val = 0;
  if (!name || name[0] == '\0' || (name[0] == '/' && name[1] == '\0')) {
    errno = EINVAL;
    return SEM_FAILED;
  }
  if (oflag & O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    (void)va_arg(ap, int);
    val = va_arg(ap, unsigned int);
    va_end(ap);
  }
  s = (sem_t *)calloc(1, sizeof(*s));
  if (!s) {
    errno = ENOMEM;
    return SEM_FAILED;
  }
  if (sem_init(s, 0, val) != 0) {
    free(s);
    return SEM_FAILED;
  }
  return s;
}

/** @brief Close a named semaphore on Emscripten. */
int sem_close(sem_t *sem) {
  if (!sem) {
    errno = EINVAL;
    return -1;
  }
  (void)sem_destroy(sem);
  free(sem);
  return 0;
}

/** @brief Unlink a named semaphore on Emscripten. */
int sem_unlink(const char *name) {
  if (!name || name[0] == '\0') {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif /* !__EMSCRIPTEN_PTHREADS__ */
#endif /* __EMSCRIPTEN__ */

#elif defined(__DOS__) || defined(__MSDOS__) ||                                \
    (defined(__WATCOMC__) && !defined(_WIN32))

#define POSIX_DOS_MAX_TLS_KEYS 64
#define POSIX_DOS_MAX_THREADS 16

struct posix_dos_attr_s {
  int detachstate;
  size_t guardsize;
  int inheritsched;
  struct sched_param schedparam;
  int schedpolicy;
  int scope;
  void *stackaddr;
  size_t stacksize;
};

struct posix_dos_mutex_s {
  int type;
  size_t owner_tid;
  int depth;
};

struct posix_dos_rwlock_s {
  int readers;
  size_t writer_tid;
};

struct posix_dos_barrier_s {
  unsigned count;
  unsigned limit;
};

static int g_dos_tls_used[POSIX_DOS_MAX_TLS_KEYS];
static void *g_dos_tls_values[POSIX_DOS_MAX_TLS_KEYS];
static void (*g_dos_tls_destructors[POSIX_DOS_MAX_TLS_KEYS])(void *);

static size_t g_dos_current_tid = 1;
static size_t g_dos_next_tid = 2;
static void *g_dos_thread_retvals[POSIX_DOS_MAX_THREADS];
static int g_dos_in_worker = 0;
static jmp_buf g_dos_exit_jmp;
static void *g_dos_exit_val = NULL;
static struct _pthread_cleanup_buffer *g_dos_cleanup_top = NULL;
static int g_dos_concurrency = 0;
static int g_dos_sched_policy = SCHED_OTHER;
static int g_dos_sched_prio = 0;

/** @brief Register fork handlers (no-op on single-process DOS). */
int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                   void (*child)(void)) {
  (void)prepare;
  (void)parent;
  (void)child;
  return 0;
}

/** @brief Initialize thread attributes on DOS. */
int pthread_attr_init(pthread_attr_t *attr) {
  struct posix_dos_attr_s *a;
  if (!attr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)calloc(1, sizeof(*a));
  if (!a) {
    return ENOMEM;
  }
  a->detachstate = PTHREAD_CREATE_JOINABLE;
  a->guardsize = 4096;
  a->schedpolicy = SCHED_OTHER;
  a->stacksize = 16384;
  attr->ptr = a;
  return 0;
}

/** @brief Destroy thread attributes on DOS. */
int pthread_attr_destroy(pthread_attr_t *attr) {
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  free(attr->ptr);
  attr->ptr = NULL;
  return 0;
}

/** @brief Get detach state from thread attributes on DOS. */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !detachstate) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *detachstate = a->detachstate;
  return 0;
}

/** @brief Set detach state on thread attributes on DOS. */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr ||
      (detachstate != PTHREAD_CREATE_JOINABLE &&
       detachstate != PTHREAD_CREATE_DETACHED)) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->detachstate = detachstate;
  return 0;
}

/** @brief Get guard size from thread attributes on DOS. */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !guardsize) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *guardsize = a->guardsize;
  return 0;
}

/** @brief Set guard size on thread attributes on DOS. */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->guardsize = guardsize;
  return 0;
}

/** @brief Get scheduling inheritance from thread attributes on DOS. */
int pthread_attr_getinheritsched(const pthread_attr_t *attr,
                                 int *inheritsched) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !inheritsched) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *inheritsched = a->inheritsched;
  return 0;
}

/** @brief Set scheduling inheritance on thread attributes on DOS. */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->inheritsched = inheritsched;
  return 0;
}

/** @brief Get scheduling parameters from thread attributes on DOS. */
int pthread_attr_getschedparam(const pthread_attr_t *attr,
                               struct sched_param *param) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !param) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *param = a->schedparam;
  return 0;
}

/** @brief Set scheduling parameters on thread attributes on DOS. */
int pthread_attr_setschedparam(pthread_attr_t *attr,
                               const struct sched_param *param) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !param) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->schedparam = *param;
  return 0;
}

/** @brief Get scheduling policy from thread attributes on DOS. */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !policy) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *policy = a->schedpolicy;
  return 0;
}

/** @brief Set scheduling policy on thread attributes on DOS. */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->schedpolicy = policy;
  return 0;
}

/** @brief Get contention scope from thread attributes on DOS. */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !contentionscope) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *contentionscope = a->scope;
  return 0;
}

/** @brief Set contention scope on thread attributes on DOS. */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || (contentionscope != 0 && contentionscope != 1)) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->scope = contentionscope;
  return 0;
}

/** @brief Get stack address and size from thread attributes on DOS. */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr,
                          size_t *stacksize) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !stackaddr || !stacksize) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *stackaddr = a->stackaddr;
  *stacksize = a->stacksize;
  return 0;
}

/** @brief Set stack address and size on thread attributes on DOS. */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->stackaddr = stackaddr;
  a->stacksize = stacksize;
  return 0;
}

/** @brief Get stack size from thread attributes on DOS. */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
  const struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr || !stacksize) {
    return EINVAL;
  }
  a = (const struct posix_dos_attr_s *)attr->ptr;
  *stacksize = a->stacksize;
  return 0;
}

/** @brief Set stack size on thread attributes on DOS. */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
  struct posix_dos_attr_s *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct posix_dos_attr_s *)attr->ptr;
  a->stacksize = stacksize;
  return 0;
}

/** @brief Initialize mutex attributes on DOS. */
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = (void *)(size_t)(PTHREAD_MUTEX_NORMAL + 1);
  return 0;
}

/** @brief Destroy mutex attributes on DOS. */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Get mutex priority ceiling on DOS. */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
                                     int *prioceiling) {
  if (!attr || !prioceiling) {
    errno = EINVAL;
    return -1;
  }
  *prioceiling = 0;
  return 0;
}

/** @brief Set mutex priority ceiling on DOS. */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
                                     int prioceiling) {
  (void)prioceiling;
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Get mutex protocol on DOS. */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
                                  int *protocol) {
  if (!attr || !protocol) {
    errno = EINVAL;
    return -1;
  }
  *protocol = 0;
  return 0;
}

/** @brief Set mutex protocol on DOS. */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
  (void)protocol;
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Get mutex process-shared attribute on DOS. */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr,
                                 int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Set mutex process-shared attribute on DOS. */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Get mutex type attribute on DOS. */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
  if (!attr || !attr->ptr || !type) {
    errno = EINVAL;
    return -1;
  }
  *type = (int)((size_t)attr->ptr - 1);
  return 0;
}

/** @brief Set mutex type attribute on DOS. */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
  if (!attr ||
      (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE &&
       type != PTHREAD_MUTEX_ERRORCHECK)) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = (void *)(size_t)(type + 1);
  return 0;
}

static struct posix_dos_mutex_s *dos_get_mutex(pthread_mutex_t *mutex) {
  struct posix_dos_mutex_s *m;
  if (!mutex) {
    return NULL;
  }
  if (!mutex->p) {
    m = (struct posix_dos_mutex_s *)calloc(1, sizeof(*m));
    if (!m) {
      return NULL;
    }
    m->type = PTHREAD_MUTEX_NORMAL;
    mutex->p = m;
  }
  return (struct posix_dos_mutex_s *)mutex->p;
}

/** @brief Initialize mutex on DOS. */
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
  struct posix_dos_mutex_s *m;
  if (!mutex) {
    errno = EINVAL;
    return EINVAL;
  }
  m = (struct posix_dos_mutex_s *)calloc(1, sizeof(*m));
  if (!m) {
    errno = ENOMEM;
    return ENOMEM;
  }
  if (attr && attr->ptr) {
    m->type = (int)((size_t)attr->ptr - 1);
  } else {
    m->type = PTHREAD_MUTEX_NORMAL;
  }
  mutex->p = m;
  return 0;
}

/** @brief Destroy mutex on DOS. */
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
  if (!mutex) {
    errno = EINVAL;
    return EINVAL;
  }
  if (mutex->p) {
    free(mutex->p);
    mutex->p = NULL;
  }
  return 0;
}

/** @brief Lock mutex on DOS. */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
  struct posix_dos_mutex_s *m = dos_get_mutex(mutex);
  if (!m) {
    errno = EINVAL;
    return EINVAL;
  }
  if (m->depth > 0 && m->owner_tid == g_dos_current_tid) {
    if (m->type == PTHREAD_MUTEX_ERRORCHECK) {
      return EBUSY;
    }
    m->depth++;
    return 0;
  }
  if (m->depth == 0) {
    m->owner_tid = g_dos_current_tid;
  }
  m->depth++;
  return 0;
}

/** @brief Try-lock mutex on DOS. */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  struct posix_dos_mutex_s *m = dos_get_mutex(mutex);
  if (!m) {
    errno = EINVAL;
    return EINVAL;
  }
  if (m->depth > 0) {
    if (m->owner_tid == g_dos_current_tid &&
        m->type == PTHREAD_MUTEX_RECURSIVE) {
      m->depth++;
      return 0;
    }
    return EBUSY;
  }
  m->owner_tid = g_dos_current_tid;
  m->depth = 1;
  return 0;
}

/** @brief Lock mutex with timeout on DOS. */
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abstime) {
  int rc;
  if (!mutex || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  rc = pthread_mutex_trylock(mutex);
  if (rc == EBUSY) {
    return ETIMEDOUT;
  }
  return rc;
}

/** @brief Unlock mutex on DOS. */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  struct posix_dos_mutex_s *m;
  if (!mutex || !mutex->p) {
    errno = EINVAL;
    return EINVAL;
  }
  m = (struct posix_dos_mutex_s *)mutex->p;
  if (m->depth <= 0) {
    if (m->type == PTHREAD_MUTEX_ERRORCHECK) {
      return EINVAL;
    }
    return 0;
  }
  m->depth--;
  if (m->depth == 0) {
    m->owner_tid = 0;
  }
  return 0;
}

/** @brief Push a cleanup handler onto the DOS thread cleanup stack. */
void _posix_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer,
                                 void (*routine)(void *), void *arg) {
  if (!buffer) {
    return;
  }
  buffer->routine = routine;
  buffer->arg = arg;
  buffer->next = g_dos_cleanup_top;
  g_dos_cleanup_top = buffer;
}

/** @brief Pop and optionally execute top cleanup handler on DOS. */
void _posix_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer,
                                int execute) {
  if (!buffer) {
    return;
  }
  if (g_dos_cleanup_top == buffer) {
    g_dos_cleanup_top = buffer->next;
  }
  if (execute && buffer->routine) {
    buffer->routine(buffer->arg);
  }
}

/** @brief Execute start_routine cooperatively on single-threaded DOS. */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  size_t saved_tid;
  size_t new_tid;
  int saved_in_worker;
  struct _pthread_cleanup_buffer *saved_cleanup;
  void *ret = NULL;
  (void)attr;
  if (!thread || !start_routine) {
    errno = EINVAL;
    return EINVAL;
  }
  new_tid = g_dos_next_tid++;
  if (new_tid >= POSIX_DOS_MAX_THREADS) {
    new_tid = 2 + (new_tid % (POSIX_DOS_MAX_THREADS - 2));
  }
  saved_tid = g_dos_current_tid;
  saved_in_worker = g_dos_in_worker;
  saved_cleanup = g_dos_cleanup_top;

  g_dos_current_tid = new_tid;
  g_dos_in_worker = 1;
  g_dos_cleanup_top = NULL;
  g_dos_exit_val = NULL;

  if (setjmp(g_dos_exit_jmp) == 0) {
    ret = start_routine(arg);
  } else {
    ret = g_dos_exit_val;
  }

  g_dos_cleanup_top = saved_cleanup;
  g_dos_in_worker = saved_in_worker;
  g_dos_current_tid = saved_tid;

  g_dos_thread_retvals[new_tid % POSIX_DOS_MAX_THREADS] = ret;
  *thread = (pthread_t)new_tid;
  return 0;
}

/** @brief Retrieve completed cooperative thread return value on DOS. */
int pthread_join(pthread_t thread, void **retval) {
  size_t tid = (size_t)thread;
  if (!thread) {
    errno = EINVAL;
    return EINVAL;
  }
  if (retval) {
    *retval = g_dos_thread_retvals[tid % POSIX_DOS_MAX_THREADS];
  }
  return 0;
}

/** @brief Mark cooperative thread handle as detached on DOS. */
int pthread_detach(pthread_t thread) {
  if (!thread) {
    errno = EINVAL;
    return EINVAL;
  }
  return 0;
}

/** @brief Initialize condition variable attributes on DOS. */
int pthread_condattr_init(pthread_condattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = (void *)1;
  return 0;
}

/** @brief Destroy condition variable attributes on DOS. */
int pthread_condattr_destroy(pthread_condattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Get clock ID from condition variable attributes on DOS. */
int pthread_condattr_getclock(const pthread_condattr_t *attr,
                              clockid_t *clock_id) {
  if (!attr || !clock_id) {
    errno = EINVAL;
    return -1;
  }
  *clock_id = (clockid_t)((size_t)attr->ptr > 0 ? (size_t)attr->ptr - 1 : 0);
  return 0;
}

/** @brief Set clock ID on condition variable attributes on DOS. */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) {
  if (!attr || clock_id < 0) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = (void *)((size_t)clock_id + 1);
  return 0;
}

/** @brief Get process-shared flag from condition variable attributes on DOS. */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Set process-shared flag on condition variable attributes on DOS. */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Initialize condition variable on DOS. */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  (void)attr;
  if (!cond) {
    errno = EINVAL;
    return EINVAL;
  }
  cond->p = (void *)1;
  return 0;
}

/** @brief Destroy condition variable on DOS. */
int pthread_cond_destroy(pthread_cond_t *cond) {
  if (!cond) {
    errno = EINVAL;
    return -1;
  }
  cond->p = NULL;
  return 0;
}

/** @brief Signal condition variable on DOS. */
int pthread_cond_signal(pthread_cond_t *cond) {
  if (!cond) {
    errno = EINVAL;
    return EINVAL;
  }
  return 0;
}

/** @brief Broadcast condition variable on DOS. */
int pthread_cond_broadcast(pthread_cond_t *cond) {
  if (!cond) {
    errno = EINVAL;
    return EINVAL;
  }
  return 0;
}

/** @brief Wait on condition variable on single-threaded DOS. */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  if (!cond || !mutex) {
    errno = EINVAL;
    return EINVAL;
  }
  return 0;
}

/** @brief Timed wait on condition variable on single-threaded DOS. */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime) {
  if (!cond || !mutex || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  return ETIMEDOUT;
}

/** @brief Initialize rwlock attributes on DOS. */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = (void *)1;
  return 0;
}

/** @brief Destroy rwlock attributes on DOS. */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Get process-shared flag from rwlock attributes on DOS. */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,
                                  int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Set process-shared flag on rwlock attributes on DOS. */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

static struct posix_dos_rwlock_s *dos_get_rwlock(pthread_rwlock_t *rwlock) {
  struct posix_dos_rwlock_s *rw;
  if (!rwlock) {
    return NULL;
  }
  if (!rwlock->p) {
    rw = (struct posix_dos_rwlock_s *)calloc(1, sizeof(*rw));
    if (!rw) {
      return NULL;
    }
    rwlock->p = rw;
  }
  return (struct posix_dos_rwlock_s *)rwlock->p;
}

/** @brief Initialize rwlock on DOS. */
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr) {
  struct posix_dos_rwlock_s *rw;
  (void)attr;
  if (!rwlock) {
    errno = EINVAL;
    return EINVAL;
  }
  rw = (struct posix_dos_rwlock_s *)calloc(1, sizeof(*rw));
  if (!rw) {
    errno = ENOMEM;
    return ENOMEM;
  }
  rwlock->p = rw;
  return 0;
}

/** @brief Destroy rwlock on DOS. */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    errno = EINVAL;
    return -1;
  }
  if (rwlock->p) {
    free(rwlock->p);
    rwlock->p = NULL;
  }
  return 0;
}

/** @brief Acquire shared read lock on DOS. */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
  struct posix_dos_rwlock_s *rw = dos_get_rwlock(rwlock);
  if (!rw) {
    errno = EINVAL;
    return EINVAL;
  }
  if (rw->writer_tid != 0) {
    return EBUSY;
  }
  rw->readers++;
  return 0;
}

/** @brief Try to acquire shared read lock on DOS. */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
  return pthread_rwlock_rdlock(rwlock);
}

/** @brief Acquire shared read lock with timeout on DOS. */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  int rc;
  if (!rwlock || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  rc = pthread_rwlock_tryrdlock(rwlock);
  return (rc == EBUSY) ? ETIMEDOUT : rc;
}

/** @brief Acquire exclusive write lock on DOS. */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
  struct posix_dos_rwlock_s *rw = dos_get_rwlock(rwlock);
  if (!rw) {
    errno = EINVAL;
    return EINVAL;
  }
  if (rw->writer_tid != 0 || rw->readers > 0) {
    return EBUSY;
  }
  rw->writer_tid = g_dos_current_tid;
  return 0;
}

/** @brief Try to acquire exclusive write lock on DOS. */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
  return pthread_rwlock_wrlock(rwlock);
}

/** @brief Acquire exclusive write lock with timeout on DOS. */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  int rc;
  if (!rwlock || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  rc = pthread_rwlock_trywrlock(rwlock);
  return (rc == EBUSY) ? ETIMEDOUT : rc;
}

/** @brief Unlock rwlock on DOS. */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
  struct posix_dos_rwlock_s *rw;
  if (!rwlock || !rwlock->p) {
    errno = EINVAL;
    return EINVAL;
  }
  rw = (struct posix_dos_rwlock_s *)rwlock->p;
  if (rw->writer_tid != 0) {
    rw->writer_tid = 0;
  } else if (rw->readers > 0) {
    rw->readers--;
  }
  return 0;
}

/** @brief Allocate thread-specific key on DOS. */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
  unsigned long i;
  if (!key) {
    errno = EINVAL;
    return EINVAL;
  }
  for (i = 0; i < POSIX_DOS_MAX_TLS_KEYS; ++i) {
    if (!g_dos_tls_used[i]) {
      g_dos_tls_used[i] = 1;
      g_dos_tls_values[i] = NULL;
      g_dos_tls_destructors[i] = destructor;
      *key = i;
      return 0;
    }
  }
  errno = EAGAIN;
  return EAGAIN;
}

/** @brief Delete thread-specific key on DOS. */
int pthread_key_delete(pthread_key_t key) {
  if (key >= POSIX_DOS_MAX_TLS_KEYS || !g_dos_tls_used[key]) {
    errno = EINVAL;
    return EINVAL;
  }
  g_dos_tls_used[key] = 0;
  g_dos_tls_values[key] = NULL;
  g_dos_tls_destructors[key] = NULL;
  return 0;
}

/** @brief Read thread-specific value on DOS. */
void *pthread_getspecific(pthread_key_t key) {
  if (key >= POSIX_DOS_MAX_TLS_KEYS || !g_dos_tls_used[key]) {
    return NULL;
  }
  return g_dos_tls_values[key];
}

/** @brief Set thread-specific value on DOS. */
int pthread_setspecific(pthread_key_t key, const void *value) {
  if (key >= POSIX_DOS_MAX_TLS_KEYS || !g_dos_tls_used[key]) {
    errno = EINVAL;
    return EINVAL;
  }
#if defined(__I86__)
  g_dos_tls_values[key] = (void *)(unsigned long)value;
#else
  g_dos_tls_values[key] = (void *)(size_t)value;
#endif
  return 0;
}

/** @brief Return thread identifier on DOS. */
pthread_t pthread_self(void) { return (pthread_t)g_dos_current_tid; }

/** @brief Compare thread identifiers on DOS. */
int pthread_equal(pthread_t t1, pthread_t t2) { return t1 == t2; }

/** @brief Terminate cooperative worker thread and run cleanup stack on DOS. */
void pthread_exit(void *retval) {
  while (g_dos_cleanup_top != NULL) {
    struct _pthread_cleanup_buffer *cb = g_dos_cleanup_top;
    g_dos_cleanup_top = cb->next;
    if (cb->routine) {
      cb->routine(cb->arg);
    }
  }
  g_dos_exit_val = retval;
  if (g_dos_in_worker) {
    longjmp(g_dos_exit_jmp, 1);
  }
}

/** @brief Cancel thread on DOS. */
int pthread_cancel(pthread_t thread) {
  if (!thread) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set cancel state on DOS. */
int pthread_setcancelstate(int state, int *oldstate) {
  if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE) {
    return EINVAL;
  }
  if (oldstate) {
    *oldstate = PTHREAD_CANCEL_ENABLE;
  }
  return 0;
}

/** @brief Set cancel type on DOS. */
int pthread_setcanceltype(int type, int *oldtype) {
  if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS) {
    return EINVAL;
  }
  if (oldtype) {
    *oldtype = PTHREAD_CANCEL_DEFERRED;
  }
  return 0;
}

/** @brief Cancellation point no-op on DOS. */
void pthread_testcancel(void) {}

/** @brief Get concurrency hint level on DOS. */
int pthread_getconcurrency(void) { return g_dos_concurrency; }

/** @brief Set concurrency hint level on DOS. */
int pthread_setconcurrency(int new_level) {
  if (new_level < 0) {
    return EINVAL;
  }
  g_dos_concurrency = new_level;
  return 0;
}

/** @brief Get CPU-time clock ID of thread on DOS. */
int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id) {
  if (!thread || !clock_id) {
    return EINVAL;
  }
  *clock_id = 0;
  return 0;
}

/** @brief Get scheduling policy and parameters of thread on DOS. */
int pthread_getschedparam(pthread_t thread, int *policy,
                          struct sched_param *param) {
  if (!thread || !policy || !param) {
    errno = EINVAL;
    return -1;
  }
  *policy = g_dos_sched_policy;
  param->sched_priority = g_dos_sched_prio;
  return 0;
}

/** @brief Set scheduling policy and parameters of thread on DOS. */
int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *param) {
  if (!thread || !param) {
    errno = EINVAL;
    return -1;
  }
  g_dos_sched_policy = policy;
  g_dos_sched_prio = param->sched_priority;
  return 0;
}

/** @brief Set scheduling priority of thread on DOS. */
int pthread_setschedprio(pthread_t thread, int prio) {
  if (!thread) {
    errno = EINVAL;
    return -1;
  }
  g_dos_sched_prio = prio;
  return 0;
}

/** @brief Set thread debugging name (no-op on DOS). */
int pthread_setname_np(pthread_t thread, const char *name) {
  if (!thread || !name) {
    return EINVAL;
  }
  return 0;
}

/** @brief Examine and change blocked signal mask on DOS. */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset) {
  (void)how;
  if (!set && !oset) {
    errno = EINVAL;
    return -1;
  }
  if (oset) {
    *oset = 0;
  }
  return 0;
}

/** @brief Initialize spinlock on DOS. */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  (void)pshared;
  if (!lock) {
    return EINVAL;
  }
  lock->ptr = (void *)1;
  return 0;
}

/** @brief Destroy spinlock on DOS. */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  lock->ptr = NULL;
  return 0;
}

/** @brief Lock spinlock on DOS. */
int pthread_spin_lock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  lock->ptr = (void *)2;
  return 0;
}

/** @brief Try-lock spinlock on DOS. */
int pthread_spin_trylock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  if (lock->ptr == (void *)2) {
    return EBUSY;
  }
  lock->ptr = (void *)2;
  return 0;
}

/** @brief Unlock spinlock on DOS. */
int pthread_spin_unlock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  lock->ptr = (void *)1;
  return 0;
}

/** @brief Initialize unnamed semaphore on DOS. */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)pshared;
  if (!sem || value > (unsigned int)SEM_VALUE_MAX) {
    errno = EINVAL;
    return -1;
  }
  sem->handle = (void *)1;
  sem->p = (void *)1;
  sem->count = (long)value;
  return 0;
}

/** @brief Destroy unnamed semaphore on DOS. */
int sem_destroy(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  sem->handle = NULL;
  sem->p = NULL;
  sem->count = 0;
  return 0;
}

/** @brief Wait (decrement) semaphore on DOS. */
int sem_wait(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (sem->count <= 0) {
    errno = EAGAIN;
    return -1;
  }
  sem->count--;
  return 0;
}

/** @brief Try-wait semaphore on DOS. */
int sem_trywait(sem_t *sem) { return sem_wait(sem); }

/** @brief Timed-wait semaphore on DOS. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  if (!sem || !sem->p || !abs_timeout || abs_timeout->tv_nsec < 0 ||
      abs_timeout->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  if (sem->count <= 0) {
    errno = ETIMEDOUT;
    return -1;
  }
  sem->count--;
  return 0;
}

/** @brief Post (increment) semaphore on DOS. */
int sem_post(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  sem->count++;
  return 0;
}

/** @brief Read semaphore value on DOS. */
int sem_getvalue(sem_t *sem, int *sval) {
  if (!sem || !sem->p || !sval) {
    errno = EINVAL;
    return -1;
  }
  *sval = (int)sem->count;
  return 0;
}

/** @brief Open or create named semaphore on DOS. */
sem_t *sem_open(const char *name, int oflag, ...) {
  sem_t *s;
  unsigned int val = 0;
  if (!name || name[0] == '\0' || (name[0] == '/' && name[1] == '\0')) {
    errno = EINVAL;
    return SEM_FAILED;
  }
  if (oflag & O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    (void)va_arg(ap, int);
    val = va_arg(ap, unsigned int);
    va_end(ap);
  }
  s = (sem_t *)calloc(1, sizeof(*s));
  if (!s) {
    errno = ENOMEM;
    return SEM_FAILED;
  }
  s->handle = (void *)1;
  s->p = (void *)1;
  s->count = (long)val;
  return s;
}

/** @brief Close named semaphore on DOS. */
int sem_close(sem_t *sem) {
  if (!sem) {
    errno = EINVAL;
    return -1;
  }
  sem->handle = NULL;
  sem->p = NULL;
  free(sem);
  return 0;
}

/** @brief Unlink named semaphore on DOS. */
int sem_unlink(const char *name) {
  if (!name || name[0] == '\0') {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Yield processor no-op on DOS. */
int sched_yield(void) { return 0; }

/** @brief Get maximum priority for scheduling policy on DOS. */
int sched_get_priority_max(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return (policy == SCHED_OTHER) ? 0 : 15;
}

/** @brief Get minimum priority for scheduling policy on DOS. */
int sched_get_priority_min(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Get scheduling parameters of process on DOS. */
int sched_getparam(pid_t pid, struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = EINVAL;
    return -1;
  }
  param->sched_priority = g_dos_sched_prio;
  return 0;
}

/** @brief Set scheduling parameters of process on DOS. */
int sched_setparam(pid_t pid, const struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = EINVAL;
    return -1;
  }
  g_dos_sched_prio = param->sched_priority;
  return 0;
}

/** @brief Get scheduling policy of process on DOS. */
int sched_getscheduler(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return -1;
  }
  return g_dos_sched_policy;
}

/** @brief Set scheduling policy and parameters of process on DOS. */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
  if (pid < 0 || !param ||
      (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR)) {
    errno = EINVAL;
    return -1;
  }
  g_dos_sched_policy = policy;
  g_dos_sched_prio = param->sched_priority;
  return 0;
}

/** @brief Get round-robin quantum interval on DOS. */
int sched_rr_get_interval(pid_t pid, struct timespec *interval) {
  if (pid < 0 || !interval) {
    errno = EINVAL;
    return -1;
  }
  interval->tv_sec = 0;
  interval->tv_nsec = 55000000L;
  return 0;
}

/** @brief Initialize barrier attributes on DOS. */
int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  attr->ptr = (void *)1;
  return 0;
}

/** @brief Destroy barrier attributes on DOS. */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Get process-shared flag from barrier attributes on DOS. */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr,
                                   int *pshared) {
  if (!attr || !pshared) {
    return EINVAL;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Set process-shared flag on barrier attributes on DOS. */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
  if (!attr || pshared != PTHREAD_PROCESS_PRIVATE) {
    return EINVAL;
  }
  return 0;
}

/** @brief Initialize barrier on DOS. */
int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr, unsigned count) {
  struct posix_dos_barrier_s *b;
  (void)attr;
  if (!barrier || count == 0) {
    errno = EINVAL;
    return EINVAL;
  }
  b = (struct posix_dos_barrier_s *)calloc(1, sizeof(*b));
  if (!b) {
    errno = ENOMEM;
    return ENOMEM;
  }
  b->count = 0;
  b->limit = count;
  barrier->ptr = b;
  return 0;
}

/** @brief Destroy barrier on DOS. */
int pthread_barrier_destroy(pthread_barrier_t *barrier) {
  if (!barrier || !barrier->ptr) {
    return EINVAL;
  }
  free(barrier->ptr);
  barrier->ptr = NULL;
  return 0;
}

/** @brief Synchronize at barrier on cooperative single-threaded DOS. */
int pthread_barrier_wait(pthread_barrier_t *barrier) {
  struct posix_dos_barrier_s *b;
  if (!barrier || !barrier->ptr) {
    return EINVAL;
  }
  b = (struct posix_dos_barrier_s *)barrier->ptr;
  b->count++;
  if (b->count >= b->limit) {
    b->count = 0;
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }
  return 0;
}

/** @brief Execute initialization routine once on DOS. */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
  if (!once_control || !init_routine) {
    errno = EINVAL;
    return EINVAL;
  }
  if (once_control->p == NULL) {
    once_control->p = (void *)1;
    init_routine();
  }
  return 0;
}

#else /* _WIN32 && !__CYGWIN__ */

#define WINAPI __stdcall
#define POSIX_WIN_INFINITE 0xFFFFFFFFUL
#define POSIX_WIN_WAIT_OBJECT_0 0UL
#define POSIX_WIN_WAIT_TIMEOUT 258UL
#define POSIX_WIN_TLS_OUT_OF_INDEXES 0xFFFFFFFFUL
#define POSIX_SEMAPHORE_ALL_ACCESS 0x1F0003UL
#define POSIX_ERROR_ALREADY_EXISTS 183UL
#define POSIX_THREAD_SET_LIMITED_INFO 0x0400UL

#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 posix_sem_uint64_t;
#elif defined(__GNUC__)
__extension__ typedef unsigned long long posix_sem_uint64_t;
#else
typedef unsigned long long posix_sem_uint64_t;
#endif

/** @brief Forward-declared Win32 FILETIME layout without including windows.h.
 */
struct posix_sem_FILETIME {
  unsigned long dwLowDateTime;  /**< Low 32 bits of 100ns intervals. */
  unsigned long dwHighDateTime; /**< High 32 bits of 100ns intervals. */
};

__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);
__declspec(dllimport) void *WINAPI GetModuleHandleA(const char *);
__declspec(dllimport) posix_pthread_farproc_t WINAPI
GetProcAddress(void *, const char *);
__declspec(dllimport) void *WINAPI CreateThread(void *, size_t,
                                                unsigned long(WINAPI *)(void *),
                                                void *, unsigned long,
                                                unsigned long *);
__declspec(dllimport) void WINAPI ExitThread(unsigned long);
__declspec(dllimport) unsigned long WINAPI WaitForSingleObject(void *,
                                                               unsigned long);
__declspec(dllimport) int WINAPI CloseHandle(void *);
__declspec(dllimport) void WINAPI Sleep(unsigned long);
__declspec(dllimport) void *WINAPI CreateSemaphoreA(void *, long, long,
                                                    const char *);
__declspec(dllimport) void *WINAPI OpenSemaphoreA(unsigned long, int,
                                                  const char *);
__declspec(dllimport) int WINAPI ReleaseSemaphore(void *, long, long *);
__declspec(dllimport) unsigned long WINAPI TlsAlloc(void);
__declspec(dllimport) int WINAPI TlsFree(unsigned long);
__declspec(dllimport) void *WINAPI TlsGetValue(unsigned long);
__declspec(dllimport) int WINAPI TlsSetValue(unsigned long, void *);
__declspec(dllimport) unsigned long WINAPI GetLastError(void);
__declspec(dllimport) void WINAPI DeleteCriticalSection(void *);
__declspec(dllimport) void WINAPI EnterCriticalSection(void *);
__declspec(dllimport) void WINAPI LeaveCriticalSection(void *);
__declspec(dllimport) unsigned char WINAPI TryEnterCriticalSection(void *);
__declspec(dllimport) int WINAPI
InitializeCriticalSectionAndSpinCount(void *, unsigned long);
__declspec(dllimport) void *WINAPI OpenThread(unsigned long, int,
                                              unsigned long);
__declspec(dllimport) void WINAPI
GetSystemTimeAsFileTime(struct posix_sem_FILETIME *);

#if defined(_MSC_VER)
#if _MSC_VER >= 1400
void *_InterlockedCompareExchangePointer(void *volatile *, void *, void *);
void *_InterlockedExchangePointer(void *volatile *, void *);
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedExchangePointer)
#define POSIX_CAS_PTR(dst, val, cmp)                                           \
  _InterlockedCompareExchangePointer((dst), (val), (cmp))
#define POSIX_EXCHANGE_PTR(dst, val) _InterlockedExchangePointer((dst), (val))
#else
__declspec(dllimport) void *
    WINAPI InterlockedCompareExchangePointer(void *volatile *, void *, void *);
__declspec(dllimport) void *WINAPI InterlockedExchangePointer(void *volatile *,
                                                              void *);
#define POSIX_CAS_PTR(dst, val, cmp)                                           \
  InterlockedCompareExchangePointer((dst), (val), (cmp))
#define POSIX_EXCHANGE_PTR(dst, val) InterlockedExchangePointer((dst), (val))
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define POSIX_CAS_PTR(dst, val, cmp)                                           \
  __sync_val_compare_and_swap((dst), (cmp), (val))
#define POSIX_EXCHANGE_PTR(dst, val)                                           \
  (void)__sync_lock_test_and_set((dst), (val))
#else
__declspec(dllimport) void *
    WINAPI InterlockedCompareExchangePointer(void *volatile *, void *, void *);
__declspec(dllimport) void *WINAPI InterlockedExchangePointer(void *volatile *,
                                                              void *);
#define POSIX_CAS_PTR(dst, val, cmp)                                           \
  InterlockedCompareExchangePointer((dst), (val), (cmp))
#define POSIX_EXCHANGE_PTR(dst, val) InterlockedExchangePointer((dst), (val))
#endif

static void *default_malloc(size_t size) { return malloc(size); }
static void default_free(void *ptr) { free(ptr); }
static void *default_get_module_handle_a(const char *name) {
  return GetModuleHandleA(name);
}
static posix_pthread_farproc_t default_get_proc_address(void *mod,
                                                        const char *name) {
  return GetProcAddress(mod, name);
}
static unsigned long default_get_current_thread_id(void) {
  return GetCurrentThreadId();
}
static void *
default_create_thread(size_t stack_size,
                      unsigned long(POSIX_PTHREAD_WINAPI *start_fn)(void *),
                      void *arg) {
  return CreateThread(NULL, stack_size, start_fn, arg, 0, NULL);
}
static void default_exit_thread(unsigned long code) { ExitThread(code); }
static unsigned long default_wait_for_single_object(void *handle,
                                                    unsigned long ms) {
  return WaitForSingleObject(handle, ms);
}
static int default_close_handle(void *handle) { return CloseHandle(handle); }
static void default_sleep_ms(unsigned long ms) { Sleep(ms); }
static unsigned long default_tls_alloc(void) { return TlsAlloc(); }
static int default_tls_free(unsigned long idx) { return TlsFree(idx); }
static void *default_tls_get_value(unsigned long idx) {
  return TlsGetValue(idx);
}
static int default_tls_set_value(unsigned long idx, void *val) {
  return TlsSetValue(idx, val);
}
static unsigned long default_get_last_error(void) { return GetLastError(); }
static void *default_open_thread(unsigned long access, int inherit,
                                 unsigned long tid) {
  return OpenThread(access, inherit, tid);
}
static int default_init_cs_spin(void *cs, unsigned long spin) {
  return InitializeCriticalSectionAndSpinCount(cs, spin);
}
static void default_delete_cs(void *cs) { DeleteCriticalSection(cs); }
static void default_enter_cs(void *cs) { EnterCriticalSection(cs); }
static void default_leave_cs(void *cs) { LeaveCriticalSection(cs); }
static unsigned char default_try_enter_cs(void *cs) {
  return TryEnterCriticalSection(cs);
}
static void *default_create_semaphore_a(long initial_count, long max_count,
                                        const char *name) {
  return CreateSemaphoreA(NULL, initial_count, max_count, name);
}
static void *default_open_semaphore_a(unsigned long access, int inherit,
                                      const char *name) {
  return OpenSemaphoreA(access, inherit, name);
}
static int default_release_semaphore(void *sem, long release_count,
                                     long *prev_count) {
  return ReleaseSemaphore(sem, release_count, prev_count);
}
static void default_get_system_time_as_filetime(unsigned long *low_dt,
                                                unsigned long *high_dt) {
  struct posix_sem_FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  *low_dt = ft.dwLowDateTime;
  *high_dt = ft.dwHighDateTime;
}
static void *default_cas_ptr(void *volatile *dst, void *exchange,
                             void *comparand) {
  return POSIX_CAS_PTR(dst, exchange, comparand);
}
static void default_exchange_ptr(void *volatile *dst, void *val) {
  (void)POSIX_EXCHANGE_PTR(dst, val);
}

static const struct posix_pthread_win32_hooks g_default_hooks = {
    default_malloc,
    default_free,
    default_get_module_handle_a,
    default_get_proc_address,
    default_get_current_thread_id,
    default_create_thread,
    default_exit_thread,
    default_wait_for_single_object,
    default_close_handle,
    default_sleep_ms,
    default_tls_alloc,
    default_tls_free,
    default_tls_get_value,
    default_tls_set_value,
    default_get_last_error,
    default_open_thread,
    default_init_cs_spin,
    default_delete_cs,
    default_enter_cs,
    default_leave_cs,
    default_try_enter_cs,
    default_create_semaphore_a,
    default_open_semaphore_a,
    default_release_semaphore,
    default_get_system_time_as_filetime,
    default_cas_ptr,
    default_exchange_ptr};

static struct posix_pthread_win32_hooks g_hooks = {
    default_malloc,
    default_free,
    default_get_module_handle_a,
    default_get_proc_address,
    default_get_current_thread_id,
    default_create_thread,
    default_exit_thread,
    default_wait_for_single_object,
    default_close_handle,
    default_sleep_ms,
    default_tls_alloc,
    default_tls_free,
    default_tls_get_value,
    default_tls_set_value,
    default_get_last_error,
    default_open_thread,
    default_init_cs_spin,
    default_delete_cs,
    default_enter_cs,
    default_leave_cs,
    default_try_enter_cs,
    default_create_semaphore_a,
    default_open_semaphore_a,
    default_release_semaphore,
    default_get_system_time_as_filetime,
    default_cas_ptr,
    default_exchange_ptr};

/** @brief Cached kernel32 dynamic procedure table. */
struct posix_pthread_dyn_cache {
  int init_set_thread_desc;
  posix_pthread_farproc_t pfn_set_thread_desc;
  int init_srw_init;
  posix_pthread_farproc_t pfn_srw_init;
  int init_srw_acq_excl;
  posix_pthread_farproc_t pfn_srw_acq_excl;
  int init_srw_rel_excl;
  posix_pthread_farproc_t pfn_srw_rel_excl;
  int init_srw_acq_sh;
  posix_pthread_farproc_t pfn_srw_acq_sh;
  int init_srw_rel_sh;
  posix_pthread_farproc_t pfn_srw_rel_sh;
  int init_srw_try_excl;
  posix_pthread_farproc_t pfn_srw_try_excl;
  int init_srw_try_sh;
  posix_pthread_farproc_t pfn_srw_try_sh;
  int init_cv_init;
  posix_pthread_farproc_t pfn_cv_init;
  int init_cv_sleep;
  posix_pthread_farproc_t pfn_cv_sleep;
  int init_cv_wake;
  posix_pthread_farproc_t pfn_cv_wake;
  int init_cv_wake_all;
  posix_pthread_farproc_t pfn_cv_wake_all;
};

static struct posix_pthread_dyn_cache g_dyn_cache;
static unsigned long g_real_cleanup_tls_index = POSIX_WIN_TLS_OUT_OF_INDEXES;
static unsigned long g_cleanup_tls_index = POSIX_WIN_TLS_OUT_OF_INDEXES;

static void ensure_cleanup_tls_index(void) {
  if (g_real_cleanup_tls_index == POSIX_WIN_TLS_OUT_OF_INDEXES &&
      g_hooks.tls_alloc == default_tls_alloc) {
    g_real_cleanup_tls_index = default_tls_alloc();
    g_cleanup_tls_index = g_real_cleanup_tls_index;
  }
}

#ifdef _MSC_VER
#pragma push_macro("read")
#undef read
#pragma section(".CRT$XCU", read)
#pragma pop_macro("read")
static void __cdecl __init_pthread_cleanup(void) { ensure_cleanup_tls_index(); }
__declspec(allocate(".CRT$XCU")) void(__cdecl *__init_pthread_cleanup_ptr)(
    void) = __init_pthread_cleanup;
#ifdef _M_IX86
#pragma comment(linker, "/INCLUDE:___init_pthread_cleanup_ptr")
#else
#pragma comment(linker, "/INCLUDE:__init_pthread_cleanup_ptr")
#endif
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void __init_pthread_cleanup(void) {
  ensure_cleanup_tls_index();
}
#endif

/** @brief Populate out_hooks with default Win32 and CRT implementations. */
void posix_pthread_get_default_win32_hooks(
    struct posix_pthread_win32_hooks *out_hooks) {
  if (out_hooks == NULL) {
    return;
  }
  *out_hooks = g_default_hooks;
}

/** @brief Install custom Win32/allocator hooks or restore defaults on NULL. */
void posix_pthread_set_win32_hooks(
    const struct posix_pthread_win32_hooks *hooks) {
  memset(&g_dyn_cache, 0, sizeof(g_dyn_cache));
  if (hooks == NULL) {
    g_hooks = g_default_hooks;
    ensure_cleanup_tls_index();
    g_cleanup_tls_index = g_real_cleanup_tls_index;
  } else {
    g_hooks = *hooks;
    if (g_hooks.tls_alloc != default_tls_alloc) {
      g_cleanup_tls_index = g_hooks.tls_alloc();
    } else {
      ensure_cleanup_tls_index();
      g_cleanup_tls_index = g_real_cleanup_tls_index;
    }
  }
}

static posix_pthread_farproc_t
resolve_kernel32_proc(int *init_flag, posix_pthread_farproc_t *cached_pfn,
                      const char *name) {
  if (!*init_flag) {
    void *mod = g_hooks.get_module_handle_a("kernel32.dll");
    if (mod != NULL) {
      *cached_pfn = g_hooks.get_proc_address(mod, name);
    }
    *init_flag = 1;
  }
  return *cached_pfn;
}

typedef long(WINAPI *PFN_SetThreadDescription)(void *, const wchar_t *);
typedef void(WINAPI *PFN_InitializeSRWLock)(void **);
typedef void(WINAPI *PFN_AcquireSRWLockExclusive)(void **);
typedef void(WINAPI *PFN_ReleaseSRWLockExclusive)(void **);
typedef void(WINAPI *PFN_AcquireSRWLockShared)(void **);
typedef void(WINAPI *PFN_ReleaseSRWLockShared)(void **);
typedef unsigned char(WINAPI *PFN_TryAcquireSRWLockExclusive)(void **);
typedef unsigned char(WINAPI *PFN_TryAcquireSRWLockShared)(void **);
typedef void(WINAPI *PFN_InitializeConditionVariable)(void **);
typedef int(WINAPI *PFN_SleepConditionVariableSRW)(void **, void **,
                                                   unsigned long,
                                                   unsigned long);
typedef void(WINAPI *PFN_WakeConditionVariable)(void **);
typedef void(WINAPI *PFN_WakeAllConditionVariable)(void **);

static int WINAPI dyn_SetThreadDescription(void *a0, const wchar_t *a1) {
  PFN_SetThreadDescription pfn =
      (PFN_SetThreadDescription)resolve_kernel32_proc(
          &g_dyn_cache.init_set_thread_desc, &g_dyn_cache.pfn_set_thread_desc,
          "SetThreadDescription");
  if (pfn != NULL) {
    pfn(a0, a1);
    return 0;
  }
  return EINVAL;
}

static void WINAPI dyn_InitializeSRWLock(void **a0) {
  PFN_InitializeSRWLock pfn = (PFN_InitializeSRWLock)resolve_kernel32_proc(
      &g_dyn_cache.init_srw_init, &g_dyn_cache.pfn_srw_init,
      "InitializeSRWLock");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static void WINAPI dyn_AcquireSRWLockExclusive(void **a0) {
  PFN_AcquireSRWLockExclusive pfn =
      (PFN_AcquireSRWLockExclusive)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_acq_excl, &g_dyn_cache.pfn_srw_acq_excl,
          "AcquireSRWLockExclusive");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static void WINAPI dyn_ReleaseSRWLockExclusive(void **a0) {
  PFN_ReleaseSRWLockExclusive pfn =
      (PFN_ReleaseSRWLockExclusive)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_rel_excl, &g_dyn_cache.pfn_srw_rel_excl,
          "ReleaseSRWLockExclusive");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static void WINAPI dyn_AcquireSRWLockShared(void **a0) {
  PFN_AcquireSRWLockShared pfn =
      (PFN_AcquireSRWLockShared)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_acq_sh, &g_dyn_cache.pfn_srw_acq_sh,
          "AcquireSRWLockShared");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static void WINAPI dyn_ReleaseSRWLockShared(void **a0) {
  PFN_ReleaseSRWLockShared pfn =
      (PFN_ReleaseSRWLockShared)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_rel_sh, &g_dyn_cache.pfn_srw_rel_sh,
          "ReleaseSRWLockShared");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static unsigned char WINAPI dyn_TryAcquireSRWLockExclusive(void **a0) {
  PFN_TryAcquireSRWLockExclusive pfn =
      (PFN_TryAcquireSRWLockExclusive)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_try_excl, &g_dyn_cache.pfn_srw_try_excl,
          "TryAcquireSRWLockExclusive");
  if (pfn != NULL) {
    return pfn(a0);
  }
  return 0;
}

static unsigned char WINAPI dyn_TryAcquireSRWLockShared(void **a0) {
  PFN_TryAcquireSRWLockShared pfn =
      (PFN_TryAcquireSRWLockShared)resolve_kernel32_proc(
          &g_dyn_cache.init_srw_try_sh, &g_dyn_cache.pfn_srw_try_sh,
          "TryAcquireSRWLockShared");
  if (pfn != NULL) {
    return pfn(a0);
  }
  return 0;
}

static void WINAPI dyn_InitializeConditionVariable(void **a0) {
  PFN_InitializeConditionVariable pfn =
      (PFN_InitializeConditionVariable)resolve_kernel32_proc(
          &g_dyn_cache.init_cv_init, &g_dyn_cache.pfn_cv_init,
          "InitializeConditionVariable");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static int WINAPI dyn_SleepConditionVariableSRW(void **a0, void **a1,
                                                unsigned long a2,
                                                unsigned long a3) {
  PFN_SleepConditionVariableSRW pfn =
      (PFN_SleepConditionVariableSRW)resolve_kernel32_proc(
          &g_dyn_cache.init_cv_sleep, &g_dyn_cache.pfn_cv_sleep,
          "SleepConditionVariableSRW");
  if (pfn != NULL) {
    return pfn(a0, a1, a2, a3);
  }
  return 0;
}

static void WINAPI dyn_WakeConditionVariable(void **a0) {
  PFN_WakeConditionVariable pfn =
      (PFN_WakeConditionVariable)resolve_kernel32_proc(
          &g_dyn_cache.init_cv_wake, &g_dyn_cache.pfn_cv_wake,
          "WakeConditionVariable");
  if (pfn != NULL) {
    pfn(a0);
  }
}

static void WINAPI dyn_WakeAllConditionVariable(void **a0) {
  PFN_WakeAllConditionVariable pfn =
      (PFN_WakeAllConditionVariable)resolve_kernel32_proc(
          &g_dyn_cache.init_cv_wake_all, &g_dyn_cache.pfn_cv_wake_all,
          "WakeAllConditionVariable");
  if (pfn != NULL) {
    pfn(a0);
  }
}

/** @brief Linked-list entry for registered pthread_atfork callbacks. */
typedef struct posix_pthread_atfork_node {
  void (*prepare)(void);                  /**< Pre-fork handler. */
  void (*parent)(void);                   /**< Post-fork parent handler. */
  void (*child)(void);                    /**< Post-fork child handler. */
  struct posix_pthread_atfork_node *next; /**< Next registered entry. */
} posix_pthread_atfork_node_t;

static posix_pthread_atfork_node_t *g_atfork_head = NULL;
static posix_pthread_atfork_node_t *g_atfork_tail = NULL;
static pthread_mutex_t g_atfork_mutex = {NULL};

/** @brief Register fork handlers to run around process duplication. */
int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                   void (*child)(void)) {
  posix_pthread_atfork_node_t *node;

  node = (posix_pthread_atfork_node_t *)g_hooks.malloc_fn(
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
  if (!node) {
    return;
  }
  run_prepare_lifo(node->next);
  if (node->prepare) {
    node->prepare();
  }
}

/** @brief Execute registered prepare fork handlers in LIFO order. */
void posix_pthread_atfork_prepare(void) {
  pthread_mutex_lock(&g_atfork_mutex);
  run_prepare_lifo(g_atfork_head);
  pthread_mutex_unlock(&g_atfork_mutex);
}

/** @brief Execute registered parent fork handlers in FIFO order. */
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

/** @brief Execute registered child fork handlers in FIFO order. */
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

/** @brief Internal thread attribute backing structure. */
struct _posix_pthread_attr {
  int detachstate;               /**< Detach state flag. */
  size_t stacksize;              /**< Requested stack size in bytes. */
  size_t guardsize;              /**< Guard page size in bytes. */
  int inheritsched;              /**< Scheduling inheritance mode. */
  int schedpolicy;               /**< Scheduling policy. */
  int contentionscope;           /**< Contention scope. */
  struct sched_param schedparam; /**< Scheduling priority parameter. */
  void *stackaddr;               /**< User-supplied stack base pointer. */
};

/** @brief Destroy a thread attributes object. */
int pthread_attr_destroy(pthread_attr_t *attr) {
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  g_hooks.free_fn(attr->ptr);
  attr->ptr = NULL;
  return 0;
}

/** @brief Retrieve detach state attribute from thread attributes. */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !detachstate) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *detachstate = a->detachstate;
  return 0;
}

/** @brief Retrieve stack guard size from thread attributes. */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !guardsize) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *guardsize = a->guardsize;
  return 0;
}

/** @brief Retrieve scheduling inheritance policy from thread attributes. */
int pthread_attr_getinheritsched(const pthread_attr_t *attr,
                                 int *inheritsched) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !inheritsched) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *inheritsched = a->inheritsched;
  return 0;
}

/** @brief Retrieve scheduling parameters from thread attributes. */
int pthread_attr_getschedparam(const pthread_attr_t *attr,
                               struct sched_param *param) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !param) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *param = a->schedparam;
  return 0;
}

/** @brief Retrieve scheduling policy from thread attributes. */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !policy) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *policy = a->schedpolicy;
  return 0;
}

/** @brief Retrieve contention scope from thread attributes. */
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !contentionscope) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *contentionscope = a->contentionscope;
  return 0;
}

/** @brief Retrieve stack address and size from thread attributes. */
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr,
                          size_t *stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !stackaddr || !stacksize) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *stackaddr = a->stackaddr;
  *stacksize = a->stacksize;
  return 0;
}

/** @brief Retrieve stack size from thread attributes. */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !stacksize) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  *stacksize = a->stacksize;
  return 0;
}

/** @brief Initialize a thread attributes object with defaults. */
int pthread_attr_init(pthread_attr_t *attr) {
  struct _posix_pthread_attr *a;
  if (!attr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)g_hooks.malloc_fn(sizeof(*a));
  if (!a) {
    return ENOMEM;
  }
  a->detachstate = PTHREAD_CREATE_JOINABLE;
  a->stacksize = 0;
  a->guardsize = 4096;
  a->inheritsched = 0;
  a->schedpolicy = 0;
  a->contentionscope = 0;
  a->schedparam.sched_priority = 0;
  a->stackaddr = NULL;
  attr->ptr = a;
  return 0;
}

/** @brief Set detach state on thread attributes. */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  if (detachstate != PTHREAD_CREATE_JOINABLE &&
      detachstate != PTHREAD_CREATE_DETACHED) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->detachstate = detachstate;
  return 0;
}

/** @brief Set stack guard size on thread attributes. */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->guardsize = guardsize;
  return 0;
}

/** @brief Set scheduling inheritance mode on thread attributes. */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->inheritsched = inheritsched;
  return 0;
}

/** @brief Set scheduling parameters on thread attributes. */
int pthread_attr_setschedparam(pthread_attr_t *attr,
                               const struct sched_param *param) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr || !param) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->schedparam = *param;
  return 0;
}

/** @brief Set scheduling policy on thread attributes. */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->schedpolicy = policy;
  return 0;
}

/** @brief Set contention scope on thread attributes. */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  if (contentionscope != 0 && contentionscope != 1) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->contentionscope = contentionscope;
  return 0;
}

/** @brief Set stack address and size on thread attributes. */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->stackaddr = stackaddr;
  a->stacksize = stacksize;
  return 0;
}

/** @brief Set stack size on thread attributes. */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
  struct _posix_pthread_attr *a;
  if (!attr || !attr->ptr) {
    return EINVAL;
  }
  a = (struct _posix_pthread_attr *)attr->ptr;
  a->stacksize = stacksize;
  return 0;
}

/** @brief Internal barrier synchronization backing state. */
struct _posix_pthread_barrier {
  void *mutex;    /**< SRWLock protecting barrier counters. */
  void *cond;     /**< CONDITION_VARIABLE for phase transition. */
  unsigned count; /**< Number of threads currently waiting. */
  unsigned limit; /**< Required thread count per phase. */
  unsigned phase; /**< Monotonic barrier generation counter. */
};

/** @brief Destroy a barrier synchronization object. */
int pthread_barrier_destroy(pthread_barrier_t *barrier) {
  if (!barrier || !barrier->ptr) {
    return EINVAL;
  }
  g_hooks.free_fn(barrier->ptr);
  barrier->ptr = NULL;
  return 0;
}

/** @brief Initialize a barrier for a given thread count. */
int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr, unsigned count) {
  struct _posix_pthread_barrier *b;
  (void)attr;
  if (!barrier || count == 0) {
    return EINVAL;
  }
  b = (struct _posix_pthread_barrier *)g_hooks.malloc_fn(sizeof(*b));
  if (!b) {
    return ENOMEM;
  }
  dyn_InitializeSRWLock(&b->mutex);
  dyn_InitializeConditionVariable(&b->cond);
  b->count = 0;
  b->limit = count;
  b->phase = 0;
  barrier->ptr = b;
  return 0;
}

/** @brief Synchronize participating threads at a barrier. */
int pthread_barrier_wait(pthread_barrier_t *barrier) {
  struct _posix_pthread_barrier *b;
  unsigned phase;
  if (!barrier || !barrier->ptr) {
    return EINVAL;
  }
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
  }
  while (phase == b->phase) {
    dyn_SleepConditionVariableSRW(&b->cond, &b->mutex, POSIX_WIN_INFINITE, 0);
  }
  dyn_ReleaseSRWLockExclusive(&b->mutex);
  return 0;
}

/** @brief Destroy a barrier attributes object. */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Get process-shared attribute from barrier attributes. */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr,
                                   int *pshared) {
  if (!attr || !pshared) {
    return EINVAL;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Initialize a barrier attributes object. */
int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Set process-shared attribute on barrier attributes. */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
  if (!attr || pshared != PTHREAD_PROCESS_PRIVATE) {
    return EINVAL;
  }
  return 0;
}

/** @brief Request cancellation of a target thread. */
int pthread_cancel(pthread_t thread) {
  if (!thread) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Push a cleanup handler onto the calling thread's cleanup stack. */
void _posix_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer,
                                 void (*routine)(void *), void *arg) {
  struct _pthread_cleanup_buffer *top;
  if (!buffer) {
    return;
  }
  buffer->routine = routine;
  buffer->arg = arg;
  buffer->next = NULL;
  ensure_cleanup_tls_index();
  if (g_cleanup_tls_index == POSIX_WIN_TLS_OUT_OF_INDEXES) {
    return;
  }
  top = (struct _pthread_cleanup_buffer *)g_hooks.tls_get_value(
      g_cleanup_tls_index);
  buffer->next = top;
  g_hooks.tls_set_value(g_cleanup_tls_index, buffer);
}

/** @brief Pop and optionally run the top cleanup handler on the stack. */
void _posix_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer,
                                int execute) {
  if (!buffer) {
    return;
  }
  if (g_cleanup_tls_index != POSIX_WIN_TLS_OUT_OF_INDEXES) {
    struct _pthread_cleanup_buffer *top =
        (struct _pthread_cleanup_buffer *)g_hooks.tls_get_value(
            g_cleanup_tls_index);
    if (top == buffer) {
      g_hooks.tls_set_value(g_cleanup_tls_index, buffer->next);
    }
  }
  if (execute && buffer->routine) {
    buffer->routine(buffer->arg);
  }
}

/** @brief Unblock all threads waiting on a condition variable. */
int pthread_cond_broadcast(pthread_cond_t *cond) {
  if (!cond) {
    return EINVAL;
  }
  dyn_WakeAllConditionVariable(&cond->p);
  return 0;
}

/** @brief Destroy a condition variable. */
int pthread_cond_destroy(pthread_cond_t *cond) {
  if (!cond) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Initialize a condition variable. */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  (void)attr;
  if (!cond) {
    return EINVAL;
  }
  dyn_InitializeConditionVariable(&cond->p);
  return 0;
}

/** @brief Unblock at least one thread waiting on a condition variable. */
int pthread_cond_signal(pthread_cond_t *cond) {
  if (!cond) {
    return EINVAL;
  }
  dyn_WakeConditionVariable(&cond->p);
  return 0;
}

/** @brief Wait on a condition variable with a timeout. */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime) {
  unsigned long timeout_ms;
  if (!cond || !mutex || !abstime) {
    return EINVAL;
  }
  timeout_ms =
      (unsigned long)(abstime->tv_sec * 1000 + abstime->tv_nsec / 1000000L);
  return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, timeout_ms, 0)
             ? 0
             : ETIMEDOUT;
}

/** @brief Wait indefinitely on a condition variable. */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  if (!cond || !mutex) {
    return EINVAL;
  }
  return dyn_SleepConditionVariableSRW(&cond->p, &mutex->p, POSIX_WIN_INFINITE,
                                       0)
             ? 0
             : EINVAL;
}

/** @brief Destroy a condition variable attributes object. */
int pthread_condattr_destroy(pthread_condattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Retrieve clock selection from condition variable attributes. */
int pthread_condattr_getclock(const pthread_condattr_t *attr,
                              clockid_t *clock_id) {
  if (!attr || !clock_id) {
    errno = EINVAL;
    return -1;
  }
  *clock_id = 0;
  return 0;
}

/** @brief Retrieve process-shared setting from condition variable attributes.
 */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Initialize a condition variable attributes object. */
int pthread_condattr_init(pthread_condattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Set clock selection on condition variable attributes. */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) {
  (void)clock_id;
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set process-shared setting on condition variable attributes. */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Start-routine thunk payload passed to Win32 CreateThread. */
struct posix_pthread_create_arg {
  void *(*start_routine)(void *); /**< User thread entry function. */
  void *arg;                      /**< User argument pointer. */
};

static unsigned long WINAPI posix_pthread_create_start(void *lpParam) {
  struct posix_pthread_create_arg *pca =
      (struct posix_pthread_create_arg *)lpParam;
  void *(*start_routine)(void *) = pca->start_routine;
  void *arg = pca->arg;
  g_hooks.free_fn(pca);
  start_routine(arg);
  return 0;
}

/** @brief Create a new thread of execution. */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  struct posix_pthread_create_arg *pca;
  void *h;
  size_t stacksize = 0;
  int detached = PTHREAD_CREATE_JOINABLE;

  if (!thread || !start_routine) {
    return EINVAL;
  }

  if (attr && attr->ptr) {
    struct _posix_pthread_attr *a = (struct _posix_pthread_attr *)attr->ptr;
    stacksize = a->stacksize;
    detached = a->detachstate;
  }

  pca = (struct posix_pthread_create_arg *)g_hooks.malloc_fn(sizeof(*pca));
  if (!pca) {
    return ENOMEM;
  }
  pca->start_routine = start_routine;
  pca->arg = arg;

  h = g_hooks.create_thread(stacksize, posix_pthread_create_start, pca);
  if (h == NULL) {
    g_hooks.free_fn(pca);
    return EAGAIN;
  }

  if (detached == PTHREAD_CREATE_DETACHED) {
    g_hooks.close_handle(h);
    *thread = NULL;
  } else {
    *thread = (pthread_t)h;
  }
  return 0;
}

/** @brief Mark a thread as detached so resources release on termination. */
int pthread_detach(pthread_t thread) {
  if (!thread) {
    return EINVAL;
  }
  g_hooks.close_handle(thread);
  return 0;
}

/** @brief Compare two thread identifiers for equality. */
int pthread_equal(pthread_t t1, pthread_t t2) { return t1 == t2; }

/** @brief Terminate the calling thread and run cleanup handlers. */
void pthread_exit(void *value_ptr) {
  ensure_cleanup_tls_index();
  if (g_cleanup_tls_index != POSIX_WIN_TLS_OUT_OF_INDEXES) {
    struct _pthread_cleanup_buffer *top =
        (struct _pthread_cleanup_buffer *)g_hooks.tls_get_value(
            g_cleanup_tls_index);
    while (top) {
      if (top->routine) {
        top->routine(top->arg);
      }
      top = top->next;
    }
    g_hooks.tls_set_value(g_cleanup_tls_index, NULL);
  }
  g_hooks.exit_thread(value_ptr ? 1UL : 0UL);
}

/** @brief Get current concurrency hint level. */
int pthread_getconcurrency(void) { return 0; }

/** @brief Obtain CPU-time clock ID of a thread. */
int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id) {
  if (!thread_id || !clock_id) {
    errno = EINVAL;
    return EINVAL;
  }
  *clock_id = 0;
  return 0;
}

/** @brief Retrieve scheduling policy and parameters of a thread. */
int pthread_getschedparam(pthread_t thread, int *policy,
                          struct sched_param *param) {
  if (!thread || !policy || !param) {
    errno = EINVAL;
    return -1;
  }
  *policy = SCHED_OTHER;
  param->sched_priority = 0;
  return 0;
}

/** @brief Read thread-specific value bound to a key. */
void *pthread_getspecific(pthread_key_t key) {
  void *val = g_hooks.tls_get_value(key);
  if (!val && g_hooks.get_last_error() != 0) {
    return NULL;
  }
  return val;
}

/** @brief Wait for thread termination. */
int pthread_join(pthread_t thread, void **value_ptr) {
  if (!thread) {
    return EINVAL;
  }
  if (g_hooks.wait_for_single_object(thread, POSIX_WIN_INFINITE) ==
      POSIX_WIN_WAIT_OBJECT_0) {
    if (value_ptr) {
      *value_ptr = NULL;
    }
    g_hooks.close_handle(thread);
    return 0;
  }
  return EINVAL;
}

/** @brief Create a thread-specific data key. */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
  unsigned long index;
  (void)destructor;
  if (!key) {
    return EINVAL;
  }
  index = g_hooks.tls_alloc();
  if (index == POSIX_WIN_TLS_OUT_OF_INDEXES) {
    return EAGAIN;
  }
  *key = index;
  return 0;
}

/** @brief Delete a thread-specific data key. */
int pthread_key_delete(pthread_key_t key) {
  return g_hooks.tls_free(key) ? 0 : EINVAL;
}

/** @brief Destroy a mutex object. */
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
  if (!mutex) {
    return EINVAL;
  }
  mutex->p = NULL;
  return 0;
}

/** @brief Initialize a mutex object. */
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
  (void)attr;
  if (!mutex) {
    return EINVAL;
  }
  dyn_InitializeSRWLock(&mutex->p);
  return 0;
}

/** @brief Acquire a mutex lock, blocking if necessary. */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
  if (!mutex) {
    return EINVAL;
  }
  dyn_AcquireSRWLockExclusive(&mutex->p);
  return 0;
}

/** @brief Acquire a mutex lock with an upper bound time limit. */
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abstime) {
  if (!mutex || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  if (dyn_TryAcquireSRWLockExclusive(&mutex->p)) {
    return 0;
  }
  return ETIMEDOUT;
}

/** @brief Attempt to acquire a mutex lock without blocking. */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  if (!mutex) {
    return EINVAL;
  }
  return dyn_TryAcquireSRWLockExclusive(&mutex->p) ? 0 : EBUSY;
}

/** @brief Release a mutex lock. */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  if (!mutex) {
    return EINVAL;
  }
  dyn_ReleaseSRWLockExclusive(&mutex->p);
  return 0;
}

/** @brief Destroy a mutex attributes object. */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Retrieve priority ceiling attribute from mutex attributes. */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
                                     int *prioceiling) {
  if (!attr || !prioceiling) {
    errno = EINVAL;
    return -1;
  }
  *prioceiling = 0;
  return 0;
}

/** @brief Retrieve priority protocol from mutex attributes. */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
                                  int *protocol) {
  if (!attr || !protocol) {
    errno = EINVAL;
    return -1;
  }
  *protocol = 0;
  return 0;
}

/** @brief Retrieve process-shared setting from mutex attributes. */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr,
                                 int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Retrieve mutex type attribute. */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
  if (!attr || !type) {
    errno = EINVAL;
    return -1;
  }
  *type = PTHREAD_MUTEX_NORMAL;
  return 0;
}

/** @brief Initialize a mutex attributes object. */
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Set priority ceiling attribute on mutex attributes. */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
                                     int prioceiling) {
  (void)prioceiling;
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set priority protocol attribute on mutex attributes. */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
  (void)protocol;
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set process-shared setting on mutex attributes. */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set mutex type attribute. */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
  if (!attr ||
      (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE &&
       type != PTHREAD_MUTEX_ERRORCHECK)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Execute an initialization routine exactly once. */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
  void *prev;
  if (!once_control || !init_routine) {
    return EINVAL;
  }
  if (once_control->p == (void *)2) {
    return 0;
  }
  prev = g_hooks.cas_ptr(&once_control->p, (void *)1, (void *)0);
  if (prev == (void *)0) {
    init_routine();
    g_hooks.exchange_ptr(&once_control->p, (void *)2);
    return 0;
  }
  if (prev == (void *)2) {
    return 0;
  }
  while (once_control->p != (void *)2) {
    g_hooks.sleep_ms(1);
  }
  return 0;
}

/** @brief Destroy a read-write lock object. */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    errno = EINVAL;
    return -1;
  }
  rwlock->p = NULL;
  return 0;
}

/** @brief Initialize a read-write lock object. */
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr) {
  (void)attr;
  if (!rwlock) {
    return EINVAL;
  }
  dyn_InitializeSRWLock(&rwlock->p);
  return 0;
}

#define POSIX_MAX_EXCL_RWLOCKS 32
static pthread_rwlock_t *g_excl_rwlocks[POSIX_MAX_EXCL_RWLOCKS];
static void *g_excl_rwlocks_lock = NULL;

static void mark_rwlock_exclusive(pthread_rwlock_t *rwlock) {
  int i;
  dyn_AcquireSRWLockExclusive(&g_excl_rwlocks_lock);
  for (i = 0; i < POSIX_MAX_EXCL_RWLOCKS; ++i) {
    if (g_excl_rwlocks[i] == NULL) {
      g_excl_rwlocks[i] = rwlock;
      break;
    }
  }
  dyn_ReleaseSRWLockExclusive(&g_excl_rwlocks_lock);
}

static int take_rwlock_exclusive(pthread_rwlock_t *rwlock) {
  int i;
  int found = 0;
  dyn_AcquireSRWLockExclusive(&g_excl_rwlocks_lock);
  for (i = 0; i < POSIX_MAX_EXCL_RWLOCKS; ++i) {
    if (g_excl_rwlocks[i] == rwlock) {
      g_excl_rwlocks[i] = NULL;
      found = 1;
      break;
    }
  }
  dyn_ReleaseSRWLockExclusive(&g_excl_rwlocks_lock);
  return found;
}

/** @brief Acquire a shared read lock on a read-write lock. */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    return EINVAL;
  }
  dyn_AcquireSRWLockShared(&rwlock->p);
  return 0;
}

/** @brief Acquire a shared read lock with timeout. */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  if (!rwlock || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  return dyn_TryAcquireSRWLockShared(&rwlock->p) ? 0 : ETIMEDOUT;
}

/** @brief Acquire an exclusive write lock with timeout. */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abstime) {
  if (!rwlock || !abstime) {
    errno = EINVAL;
    return EINVAL;
  }
  if (dyn_TryAcquireSRWLockExclusive(&rwlock->p)) {
    mark_rwlock_exclusive(rwlock);
    return 0;
  }
  return ETIMEDOUT;
}

/** @brief Attempt to acquire a shared read lock without blocking. */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    return EINVAL;
  }
  return dyn_TryAcquireSRWLockShared(&rwlock->p) ? 0 : EBUSY;
}

/** @brief Attempt to acquire an exclusive write lock without blocking. */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    return EINVAL;
  }
  if (dyn_TryAcquireSRWLockExclusive(&rwlock->p)) {
    mark_rwlock_exclusive(rwlock);
    return 0;
  }
  return EBUSY;
}

/** @brief Unlock a read-write lock held in shared or exclusive mode. */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    return EINVAL;
  }
  if (take_rwlock_exclusive(rwlock)) {
    dyn_ReleaseSRWLockExclusive(&rwlock->p);
  } else {
    dyn_ReleaseSRWLockShared(&rwlock->p);
  }
  return 0;
}

/** @brief Acquire an exclusive write lock on a read-write lock. */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
  if (!rwlock) {
    return EINVAL;
  }
  dyn_AcquireSRWLockExclusive(&rwlock->p);
  mark_rwlock_exclusive(rwlock);
  return 0;
}

/** @brief Destroy a read-write lock attributes object. */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Retrieve process-shared setting from read-write lock attributes. */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,
                                  int *pshared) {
  if (!attr || !pshared) {
    errno = EINVAL;
    return -1;
  }
  *pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

/** @brief Initialize a read-write lock attributes object. */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
  if (!attr) {
    errno = EINVAL;
    return -1;
  }
  attr->ptr = NULL;
  return 0;
}

/** @brief Set process-shared setting on read-write lock attributes. */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
  if (!attr || (pshared != PTHREAD_PROCESS_PRIVATE &&
                pshared != PTHREAD_PROCESS_SHARED)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Obtain identifier of the calling thread. */
pthread_t pthread_self(void) {
  return (pthread_t)(size_t)g_hooks.get_current_thread_id();
}

/** @brief Atomically set calling thread cancelability state. */
int pthread_setcancelstate(int state, int *oldstate) {
  if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE) {
    errno = EINVAL;
    return EINVAL;
  }
  if (oldstate) {
    *oldstate = PTHREAD_CANCEL_ENABLE;
  }
  return 0;
}

/** @brief Atomically set calling thread cancelability type. */
int pthread_setcanceltype(int type, int *oldtype) {
  if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS) {
    errno = EINVAL;
    return EINVAL;
  }
  if (oldtype) {
    *oldtype = PTHREAD_CANCEL_DEFERRED;
  }
  return 0;
}

/** @brief Set concurrency hint level. */
int pthread_setconcurrency(int new_level) {
  if (new_level < 0) {
    return EINVAL;
  }
  return 0;
}

/** @brief Update scheduling policy and parameters of a thread. */
int pthread_setschedparam(pthread_t thread, int policy,
                          const struct sched_param *param) {
  (void)policy;
  if (!thread || !param) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Update scheduling priority of a thread. */
int pthread_setschedprio(pthread_t thread, int prio) {
  (void)prio;
  if (!thread) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Assign a descriptive debugging name to a thread. */
int pthread_setname_np(pthread_t thread, const char *name) {
  wchar_t wname[256];
#if defined(_MSC_VER)
  size_t out_len = 0;
#endif
  void *hThread;
  unsigned long threadId;

  if (!thread || !name) {
    return EINVAL;
  }

#if defined(_MSC_VER)
  mbstowcs_s(&out_len, wname, 256, name, 255);
#else
  mbstowcs(wname, name, 255);
#endif
  wname[255] = L'\0';

  threadId = (unsigned long)(size_t)thread;
  hThread = g_hooks.open_thread(POSIX_THREAD_SET_LIMITED_INFO, 0, threadId);
  if (hThread) {
    int res = dyn_SetThreadDescription(hThread, wname);
    g_hooks.close_handle(hThread);
    return res;
  }

  return dyn_SetThreadDescription(thread, wname);
}

/** @brief Examine and change blocked signal mask of calling thread. */
int(pthread_sigmask)(int how, const sigset_t *set, sigset_t *oset) {
  (void)how;
  if (!set && !oset) {
    errno = EINVAL;
    return -1;
  }
  if (oset) {
    *oset = 0;
  }
  return 0;
}

/** @brief Associate a thread-specific value with a key. */
int pthread_setspecific(pthread_key_t key, const void *value) {
  return g_hooks.tls_set_value(key, (void *)(size_t)value) ? 0 : EINVAL;
}

/** @brief Destroy a spinlock object. */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  g_hooks.delete_cs(lock->ptr);
  g_hooks.free_fn(lock->ptr);
  lock->ptr = NULL;
  return 0;
}

/** @brief Initialize a spinlock object. */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  void *cs;
  (void)pshared;
  if (!lock) {
    return EINVAL;
  }
  cs = g_hooks.malloc_fn(64);
  if (!cs) {
    return ENOMEM;
  }
  g_hooks.init_cs_spin(cs, 4000UL);
  lock->ptr = cs;
  return 0;
}

/** @brief Acquire a spinlock. */
int pthread_spin_lock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  g_hooks.enter_cs(lock->ptr);
  return 0;
}

/** @brief Attempt to acquire a spinlock without blocking. */
int pthread_spin_trylock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  return g_hooks.try_enter_cs(lock->ptr) ? 0 : EBUSY;
}

/** @brief Release a spinlock. */
int pthread_spin_unlock(pthread_spinlock_t *lock) {
  if (!lock || !lock->ptr) {
    return EINVAL;
  }
  g_hooks.leave_cs(lock->ptr);
  return 0;
}

/** @brief Create a cancellation point in the calling thread. */
void pthread_testcancel(void) {}

/** @brief Get maximum priority value for a scheduling policy. */
int sched_get_priority_max(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return 15;
}

/** @brief Get minimum priority value for a scheduling policy. */
int sched_get_priority_min(int policy) {
  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    errno = EINVAL;
    return -1;
  }
  return -15;
}

/** @brief Retrieve scheduling parameters of a process. */
int sched_getparam(pid_t pid, struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = EINVAL;
    return -1;
  }
  param->sched_priority = 0;
  return 0;
}

/** @brief Retrieve scheduling policy of a process. */
int sched_getscheduler(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return -1;
  }
  return SCHED_OTHER;
}

/** @brief Retrieve round-robin execution time quantum for a process. */
int sched_rr_get_interval(pid_t pid, struct timespec *interval) {
  if (pid < 0 || !interval) {
    errno = EINVAL;
    return -1;
  }
  interval->tv_sec = 0;
  interval->tv_nsec = 15625000L;
  return 0;
}

/** @brief Set scheduling parameters of a process. */
int sched_setparam(pid_t pid, const struct sched_param *param) {
  if (pid < 0 || !param) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Set scheduling policy and parameters of a process. */
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
  if (pid < 0 || policy < 0 || !param) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Yield processor execution to another ready thread. */
int sched_yield(void) {
  g_hooks.sleep_ms(0);
  return 0;
}

/** @brief Close a named semaphore. */
int sem_close(sem_t *sem) {
  if (!sem) {
    errno = EINVAL;
    return -1;
  }
  if (sem->p) {
    g_hooks.close_handle(sem->p);
  }
  g_hooks.free_fn(sem);
  return 0;
}

/** @brief Destroy an unnamed semaphore. */
int sem_destroy(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  g_hooks.close_handle(sem->p);
  sem->p = NULL;
  return 0;
}

/** @brief Read current value of a semaphore. */
int sem_getvalue(sem_t *sem, int *sval) {
  long prev = 0;
  if (!sem || !sem->p || !sval) {
    errno = EINVAL;
    return -1;
  }
  if (g_hooks.wait_for_single_object(sem->p, 0) == POSIX_WIN_WAIT_OBJECT_0) {
    if (g_hooks.release_semaphore(sem->p, 1, &prev)) {
      *sval = (int)prev + 1;
    } else {
      *sval = 1;
    }
  } else {
    *sval = 0;
  }
  return 0;
}

/** @brief Initialize an unnamed semaphore. */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
  void *h;
  (void)pshared;
  if (!sem || value > 2147483647U) {
    errno = EINVAL;
    return -1;
  }
  h = g_hooks.create_semaphore_a((long)value, 2147483647L, NULL);
  if (!h) {
    errno = EINVAL;
    return -1;
  }
  sem->p = h;
  return 0;
}

/** @brief Open or create a named semaphore. */
sem_t *sem_open(const char *name, int oflag, ...) {
  void *h;
  sem_t *sem;
  char win_name[260];
  unsigned int value = 0;
  int i;

  if (!name || name[0] == '\0') {
    errno = EINVAL;
    return SEM_FAILED;
  }

  if (name[0] == '/') {
    name++;
  }
  if (name[0] == '\0') {
    errno = EINVAL;
    return SEM_FAILED;
  }

  win_name[0] = '\0';
#if defined(_MSC_VER)
  strncat_s(win_name, sizeof(win_name), "Local", _TRUNCATE);
#else
  strncat(win_name, "Local", sizeof(win_name) - 1);
#endif
  win_name[5] = (char)92;
  win_name[6] = '\0';
#if defined(_MSC_VER)
  strncat_s(win_name, sizeof(win_name), name, _TRUNCATE);
#else
  strncat(win_name, name, sizeof(win_name) - strlen(win_name) - 1);
#endif

  for (i = 6; win_name[i] != '\0'; i++) {
    if (win_name[i] == '/') {
      win_name[i] = '_';
    }
  }

#ifndef O_CREAT
#define O_CREAT 0x0100
#endif
#ifndef O_EXCL
#define O_EXCL 0x0400
#endif

  if (oflag & O_CREAT) {
    va_list ap;
    int mode;
    va_start(ap, oflag);
    mode = va_arg(ap, int);
    (void)mode;
    value = va_arg(ap, unsigned int);
    va_end(ap);

    if (value > 2147483647U) {
      errno = EINVAL;
      return SEM_FAILED;
    }

    h = g_hooks.create_semaphore_a((long)value, 2147483647L, win_name);
    if (!h) {
      errno = EINVAL;
      return SEM_FAILED;
    }
    if ((oflag & O_EXCL) &&
        g_hooks.get_last_error() == POSIX_ERROR_ALREADY_EXISTS) {
      g_hooks.close_handle(h);
      errno = EEXIST;
      return SEM_FAILED;
    }
  } else {
    h = g_hooks.open_semaphore_a(POSIX_SEMAPHORE_ALL_ACCESS, 0, win_name);
    if (!h) {
      errno = ENOENT;
      return SEM_FAILED;
    }
  }

  sem = (sem_t *)g_hooks.malloc_fn(sizeof(sem_t));
  if (!sem) {
    g_hooks.close_handle(h);
    errno = ENOMEM;
    return SEM_FAILED;
  }
  sem->p = h;
  return sem;
}

/** @brief Increment (unlock) a semaphore. */
int sem_post(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (!g_hooks.release_semaphore(sem->p, 1, NULL)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Decrement (lock) a semaphore with an absolute timeout. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  unsigned long timeout = POSIX_WIN_INFINITE;
  unsigned long res;
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (abs_timeout) {
    unsigned long low_dt = 0;
    unsigned long high_dt = 0;
    posix_sem_uint64_t now_100ns;
    posix_sem_uint64_t abs_100ns;

    if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000L) {
      errno = EINVAL;
      return -1;
    }
    g_hooks.get_system_time_as_filetime(&low_dt, &high_dt);
    now_100ns = ((posix_sem_uint64_t)high_dt << 32) | low_dt;
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
  res = g_hooks.wait_for_single_object(sem->p, timeout);
  if (res == POSIX_WIN_WAIT_OBJECT_0) {
    return 0;
  }
  if (res == POSIX_WIN_WAIT_TIMEOUT) {
    errno = ETIMEDOUT;
    return -1;
  }
  errno = EINVAL;
  return -1;
}

/** @brief Attempt to decrement a semaphore without blocking. */
int sem_trywait(sem_t *sem) {
  unsigned long res;
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  res = g_hooks.wait_for_single_object(sem->p, 0);
  if (res == POSIX_WIN_WAIT_OBJECT_0) {
    return 0;
  }
  if (res == POSIX_WIN_WAIT_TIMEOUT) {
    errno = EAGAIN;
    return -1;
  }
  errno = EINVAL;
  return -1;
}

/** @brief Unlink a named semaphore. */
int sem_unlink(const char *name) {
  if (!name || name[0] == '\0') {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/** @brief Decrement (lock) a semaphore, blocking if zero. */
int sem_wait(sem_t *sem) {
  if (!sem || !sem->p) {
    errno = EINVAL;
    return -1;
  }
  if (g_hooks.wait_for_single_object(sem->p, POSIX_WIN_INFINITE) ==
      POSIX_WIN_WAIT_OBJECT_0) {
    return 0;
  }
  errno = EINVAL;
  return -1;
}

#endif /* Platform selection */

/** @brief Dummy function to prevent empty translation units. */
int dummy_posix_pthread(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_pthread;

#endif
typedef int make_iso_compilers_happy_mega_batch3_10;

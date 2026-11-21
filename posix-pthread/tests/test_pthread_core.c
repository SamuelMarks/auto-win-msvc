/* test_pthread_core.c - Strict C89 Core Pthread Lifecycle & Mock Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

struct detach_ctx {
  volatile int done;
};

static void *detach_thread_fn(void *arg) {
  struct detach_ctx *ctx = (struct detach_ctx *)arg;
  if (ctx) {
    ctx->done = 1;
  }
  return NULL;
}

static void *noop_thread_fn(void *arg) { return arg; }

static int g_exit_cleanup_count = 0;
static void exit_cleanup_cb(void *arg) {
  int *p = (int *)arg;
  if (p) {
    (*p)++;
  }
  g_exit_cleanup_count++;
}

static void *exiting_worker_fn(void *arg) {
  pthread_cleanup_push(exit_cleanup_cb, arg);
  pthread_exit(NULL);
  pthread_cleanup_pop(0);
  return NULL;
}

static int g_once_runs = 0;
static void once_cb(void) { g_once_runs++; }

#if defined(_WIN32) && !defined(__CYGWIN__)
static void *core_oom_malloc(size_t sz) {
  (void)sz;
  return NULL;
}

static void *
mock_create_thread_fail(size_t stack_size,
                        unsigned long(POSIX_PTHREAD_WINAPI *start_fn)(void *),
                        void *arg) {
  (void)stack_size;
  (void)start_fn;
  (void)arg;
  return NULL;
}

static unsigned long g_mock_exit_code = 999UL;
static void mock_exit_thread_record(unsigned long code) {
  g_mock_exit_code = code;
}

static unsigned long mock_tls_alloc_fail_core(void) { return 0xFFFFFFFFUL; }

static unsigned long g_mock_last_error = 0;
static void *mock_tls_get_null(unsigned long idx) {
  (void)idx;
  return NULL;
}
static unsigned long mock_get_last_err_fn(void) { return g_mock_last_error; }

static unsigned long mock_wait_fail(void *h, unsigned long ms) {
  (void)h;
  (void)ms;
  return 0xFFFFFFFFUL;
}

static void *mock_cas_returns_done(void *volatile *dst, void *exch, void *cmp) {
  (void)dst;
  (void)exch;
  (void)cmp;
  return (void *)2;
}

static pthread_once_t *g_spin_once_target = NULL;
static int g_spin_sleep_calls = 0;
static void *mock_cas_returns_busy(void *volatile *dst, void *exch, void *cmp) {
  (void)dst;
  (void)exch;
  (void)cmp;
  return (void *)1;
}
static void mock_sleep_completes_once(unsigned long ms) {
  (void)ms;
  g_spin_sleep_calls++;
  if (g_spin_sleep_calls >= 2 && g_spin_once_target) {
    g_spin_once_target->p = (void *)2;
  }
}

static void *mock_open_thread_null(unsigned long access, int inherit,
                                   unsigned long tid) {
  (void)access;
  (void)inherit;
  (void)tid;
  return NULL;
}
#endif

TEST test_pthread_atfork(void) {
  ASSERT_EQ(0, pthread_atfork(NULL, NULL, NULL));
  PASS();
}

TEST test_pthread_cancel(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_cancel(NULL));
  ASSERT_EQ(0, pthread_cancel(pthread_self()));
#endif
  PASS();
}

TEST test_pthread_create(void) {
  pthread_t th;
  pthread_attr_t attr;
  int payload = 42;

  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstacksize(&attr, 1024 * 1024));
  ASSERT_EQ(0, pthread_create(&th, &attr, noop_thread_fn, &payload));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    pthread_attr_t uninit_attr;

    ASSERT_EQ(EINVAL, pthread_create(NULL, NULL, noop_thread_fn, NULL));
    ASSERT_EQ(EINVAL, pthread_create(&th, NULL, NULL, NULL));

    /* attr != NULL with attr->ptr == NULL */
    uninit_attr.ptr = NULL;
    ASSERT_EQ(0, pthread_create(&th, &uninit_attr, noop_thread_fn, NULL));
    ASSERT_EQ(0, pthread_join(th, NULL));

    /* Detached creation */
    ASSERT_EQ(0, pthread_attr_init(&attr));
    ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
    ASSERT_EQ(0, pthread_create(&th, &attr, noop_thread_fn, NULL));
    ASSERT_EQ(0, pthread_attr_destroy(&attr));

    /* OOM on pca */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.malloc_fn = core_oom_malloc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(ENOMEM, pthread_create(&th, NULL, noop_thread_fn, NULL));

    /* CreateThread failure */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.create_thread = mock_create_thread_fail;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(EAGAIN, pthread_create(&th, NULL, noop_thread_fn, NULL));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_detach(void) {
  pthread_t th;
  struct detach_ctx ctx;
  int i;
  ctx.done = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, detach_thread_fn, &ctx));
  ASSERT_EQ(0, pthread_detach(th));
  while (!ctx.done) {
    sched_yield();
  }
  for (i = 0; i < 1000; ++i) {
    sched_yield();
  }
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_detach(NULL));
#endif
  PASS();
}

TEST test_pthread_equal(void) {
  pthread_t me = pthread_self();
  ASSERT(pthread_equal(me, me) != 0);
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(0, pthread_equal(me, NULL));
#endif
  PASS();
}

TEST test_pthread_exit(void) {
  pthread_t th;
  int cleaned = 0;

  g_exit_cleanup_count = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, exiting_worker_fn, &cleaned));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(1, cleaned);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    struct _pthread_cleanup_buffer b_null;
    struct _pthread_cleanup_buffer b_fn;
    int cval = 0;

    /* Mock exit_thread so we can test value_ptr != NULL, NULL routine, and TLS
     * fail */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.exit_thread = mock_exit_thread_record;
    posix_pthread_set_win32_hooks(&hooks);

    _posix_pthread_cleanup_push(&b_null, NULL, NULL);
    _posix_pthread_cleanup_push(&b_fn, exit_cleanup_cb, &cval);
    g_mock_exit_code = 999UL;
    pthread_exit((void *)1);
    ASSERT_EQ(1UL, g_mock_exit_code);
    ASSERT_EQ(1, cval);

    g_mock_exit_code = 999UL;
    pthread_exit(NULL);
    ASSERT_EQ(0UL, g_mock_exit_code);

    /* TLS_OUT_OF_INDEXES branch in pthread_exit */
    hooks.tls_alloc = mock_tls_alloc_fail_core;
    posix_pthread_set_win32_hooks(&hooks);
    g_mock_exit_code = 999UL;
    pthread_exit(NULL);
    ASSERT_EQ(0UL, g_mock_exit_code);

    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_getconcurrency(void) {
  ASSERT(pthread_getconcurrency() >= 0);
  PASS();
}

TEST test_pthread_getcpuclockid(void) {
#if !defined(__APPLE__)
  clockid_t cid = 0;
  int rc = pthread_getcpuclockid(pthread_self(), &cid);
  ASSERT_EQ(0, rc);
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_getcpuclockid(NULL, &cid));
  ASSERT_EQ(EINVAL, pthread_getcpuclockid(pthread_self(), NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_getschedparam(void) {
  int policy = 0;
  struct sched_param sp;
  ASSERT_EQ(0, pthread_getschedparam(pthread_self(), &policy, &sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_getschedparam(NULL, &policy, &sp));
  ASSERT_EQ(-1, pthread_getschedparam(pthread_self(), NULL, &sp));
  ASSERT_EQ(-1, pthread_getschedparam(pthread_self(), &policy, NULL));
#endif
  PASS();
}

TEST test_pthread_getspecific(void) {
  pthread_key_t key;
  int data = 77;
  ASSERT_EQ(0, pthread_key_create(&key, NULL));
  ASSERT_EQ((void *)NULL, pthread_getspecific(key));
  ASSERT_EQ(0, pthread_setspecific(key, &data));
  ASSERT_EQ((void *)&data, pthread_getspecific(key));
  ASSERT_EQ(0, pthread_key_delete(key));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.tls_get_value = mock_tls_get_null;
    hooks.get_last_error = mock_get_last_err_fn;
    posix_pthread_set_win32_hooks(&hooks);

    g_mock_last_error = 0;
    ASSERT_EQ((void *)NULL, pthread_getspecific(0));
    g_mock_last_error = 87UL;
    ASSERT_EQ((void *)NULL, pthread_getspecific(0));

    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_join(void) {
  pthread_t th;
  void *ret = (void *)1;
  ASSERT_EQ(0, pthread_create(&th, NULL, noop_thread_fn, NULL));
  ASSERT_EQ(0, pthread_join(th, &ret));
  ASSERT_EQ((void *)NULL, ret);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_join(NULL, &ret));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.wait_for_single_object = mock_wait_fail;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(EINVAL, pthread_join((pthread_t)(size_t)1, &ret));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_once(void) {
  pthread_once_t oc = PTHREAD_ONCE_INIT;
  g_once_runs = 0;
  ASSERT_EQ(0, pthread_once(&oc, once_cb));
  ASSERT_EQ(0, pthread_once(&oc, once_cb));
  ASSERT_EQ(1, g_once_runs);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    pthread_once_t race_oc = PTHREAD_ONCE_INIT;
    pthread_once_t spin_oc = PTHREAD_ONCE_INIT;

    ASSERT_EQ(EINVAL, pthread_once(NULL, once_cb));
    ASSERT_EQ(EINVAL, pthread_once(&oc, NULL));

    /* CAS observes already-completed (void *)2 */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.cas_ptr = mock_cas_returns_done;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(0, pthread_once(&race_oc, once_cb));

    /* CAS observes in-progress (void *)1 and spins in sleep_ms until (void *)2
     */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.cas_ptr = mock_cas_returns_busy;
    hooks.sleep_ms = mock_sleep_completes_once;
    g_spin_once_target = &spin_oc;
    g_spin_sleep_calls = 0;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(0, pthread_once(&spin_oc, once_cb));
    ASSERT_EQ(2, g_spin_sleep_calls);

    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_self(void) {
  ASSERT(pthread_self() != (pthread_t)0);
  PASS();
}

TEST test_pthread_setcancelstate(void) {
  int old = -1;
  ASSERT_EQ(0, pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old));
  ASSERT_EQ(0, pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
  ASSERT_EQ(EINVAL, pthread_setcancelstate(999, &old));
  PASS();
}

TEST test_pthread_setcanceltype(void) {
  int old = -1;
  ASSERT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old));
  ASSERT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL));
  ASSERT_EQ(EINVAL, pthread_setcanceltype(999, &old));
  PASS();
}

TEST test_pthread_setconcurrency(void) {
  ASSERT_EQ(0, pthread_setconcurrency(0));
  ASSERT_EQ(EINVAL, pthread_setconcurrency(-1));
  PASS();
}

TEST test_pthread_setschedparam(void) {
  int policy = 0;
  struct sched_param sp;
  ASSERT_EQ(0, pthread_getschedparam(pthread_self(), &policy, &sp));
  ASSERT_EQ(0, pthread_setschedparam(pthread_self(), policy, &sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_setschedparam(NULL, policy, &sp));
  ASSERT_EQ(-1, pthread_setschedparam(pthread_self(), policy, NULL));
#endif
  PASS();
}

TEST test_pthread_setschedprio(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_setschedprio(NULL, 0));
  ASSERT_EQ(0, pthread_setschedprio(pthread_self(), 0));
#endif
  PASS();
}

TEST test_pthread_setspecific(void) {
  pthread_key_t key;
  int v = 9;
  ASSERT_EQ(0, pthread_key_create(&key, NULL));
  ASSERT_EQ(0, pthread_setspecific(key, &v));
  ASSERT_EQ((void *)&v, pthread_getspecific(key));
  ASSERT_EQ(0, pthread_key_delete(key));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_setspecific(0xFFFFFFFFUL, &v));
#endif
  PASS();
}

TEST test_pthread_setname_np(void) {
#if defined(__APPLE__)
  ASSERT_EQ(0, pthread_setname_np("awm-worker"));
#elif defined(_WIN32) && !defined(__CYGWIN__)
  struct posix_pthread_win32_hooks hooks;
  int rc;
  ASSERT_EQ(EINVAL, pthread_setname_np(NULL, "awm"));
  ASSERT_EQ(EINVAL, pthread_setname_np(pthread_self(), NULL));

  /* Real OpenThread succeeds for current thread ID */
  rc = pthread_setname_np(pthread_self(), "awm-worker");
  ASSERT(rc == 0 || rc == EINVAL);

  /* OpenThread returns NULL -> falls back to direct handle
   * SetThreadDescription */
  posix_pthread_get_default_win32_hooks(&hooks);
  hooks.open_thread = mock_open_thread_null;
  posix_pthread_set_win32_hooks(&hooks);
  rc = pthread_setname_np(pthread_self(), "awm-direct");
  ASSERT(rc == 0 || rc == EINVAL);
  posix_pthread_set_win32_hooks(NULL);
#elif defined(_GNU_SOURCE) || defined(__FreeBSD__)
  (void)pthread_setname_np(pthread_self(), "awm-worker");
#endif
  PASS();
}

TEST test_pthread_sigmask(void) {
  sigset_t s;
  sigset_t os;
#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__WATCOMC__) ||       \
    defined(__MSDOS__) || defined(__DOS__)
  s = 0;
  os = 1;
  ASSERT_EQ(-1, pthread_sigmask(0, NULL, NULL));
  ASSERT_EQ(0, pthread_sigmask(0, &s, &os));
  ASSERT_EQ((sigset_t)0, os);
  ASSERT_EQ(0, pthread_sigmask(0, &s, NULL));
  ASSERT_EQ(0, pthread_sigmask(0, NULL, &os));
#else
  sigemptyset(&s);
  ASSERT_EQ(0, pthread_sigmask(SIG_BLOCK, &s, &os));
#endif
  PASS();
}

TEST test_pthread_testcancel(void) {
  pthread_testcancel();
  PASS();
}

SUITE(suite_posix_pthread_pthread_core) {
  RUN_TEST(test_pthread_atfork);
  RUN_TEST(test_pthread_cancel);
  RUN_TEST(test_pthread_create);
  RUN_TEST(test_pthread_detach);
  RUN_TEST(test_pthread_equal);
  RUN_TEST(test_pthread_exit);
  RUN_TEST(test_pthread_getconcurrency);
  RUN_TEST(test_pthread_getcpuclockid);
  RUN_TEST(test_pthread_getschedparam);
  RUN_TEST(test_pthread_getspecific);
  RUN_TEST(test_pthread_join);
  RUN_TEST(test_pthread_once);
  RUN_TEST(test_pthread_self);
  RUN_TEST(test_pthread_setcancelstate);
  RUN_TEST(test_pthread_setcanceltype);
  RUN_TEST(test_pthread_setconcurrency);
  RUN_TEST(test_pthread_setschedparam);
  RUN_TEST(test_pthread_setschedprio);
  RUN_TEST(test_pthread_setspecific);
  RUN_TEST(test_pthread_setname_np);
  RUN_TEST(test_pthread_sigmask);
  RUN_TEST(test_pthread_testcancel);
}

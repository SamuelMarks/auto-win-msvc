/* test_pthread_barrier.c - Strict C89 Barrier Synchronization Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
#include <string.h>
/* clang-format on */

#if !defined(__APPLE__)
struct barrier_worker_ctx {
  pthread_barrier_t *bar;
  int rc;
};

static void *barrier_worker_fn(void *arg) {
  struct barrier_worker_ctx *ctx = (struct barrier_worker_ctx *)arg;
  ctx->rc = pthread_barrier_wait(ctx->bar);
  return NULL;
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
static void *barrier_oom_malloc(size_t sz) {
  (void)sz;
  return NULL;
}

static void *barrier_null_mod(const char *name) {
  (void)name;
  return NULL;
}

static pthread_barrier_t *g_spurious_bar;
static int g_spurious_calls;

static int POSIX_PTHREAD_WINAPI mock_spurious_sleep_cv(void **cv, void **srw,
                                                       unsigned long ms,
                                                       unsigned long flags) {
  (void)cv;
  (void)srw;
  (void)ms;
  (void)flags;
  g_spurious_calls++;
  if (g_spurious_calls >= 2 && g_spurious_bar && g_spurious_bar->ptr) {
    /* Advance phase on second wakeup to simulate serial thread completion */
    struct {
      void *mutex;
      void *cond;
      unsigned count;
      unsigned limit;
      unsigned phase;
    } *b = g_spurious_bar->ptr;
    b->phase++;
    b->count = 0;
  }
  return 1;
}

static posix_pthread_farproc_t mock_spurious_get_proc(void *mod,
                                                      const char *name) {
  struct posix_pthread_win32_hooks def;
  if (strcmp(name, "SleepConditionVariableSRW") == 0) {
    return (posix_pthread_farproc_t)mock_spurious_sleep_cv;
  }
  posix_pthread_get_default_win32_hooks(&def);
  return def.get_proc_address(mod, name);
}
#endif

TEST test_pthread_barrier_init(void) {
#if !defined(__APPLE__)
  pthread_barrier_t bar;
  ASSERT_EQ(EINVAL, pthread_barrier_init(&bar, NULL, 0));
  ASSERT_EQ(0, pthread_barrier_init(&bar, NULL, 1));
  ASSERT_EQ(0, pthread_barrier_destroy(&bar));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_barrier_init(NULL, NULL, 1));

    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.malloc_fn = barrier_oom_malloc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(ENOMEM, pthread_barrier_init(&bar, NULL, 2));

    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.get_module_handle_a = barrier_null_mod;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(0, pthread_barrier_init(&bar, NULL, 1));
    ASSERT_EQ(PTHREAD_BARRIER_SERIAL_THREAD, pthread_barrier_wait(&bar));
    ASSERT_EQ(0, pthread_barrier_destroy(&bar));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
#endif
  PASS();
}

TEST test_pthread_barrier_destroy(void) {
#if !defined(__APPLE__)
  pthread_barrier_t bar;
  ASSERT_EQ(0, pthread_barrier_init(&bar, NULL, 1));
  ASSERT_EQ(0, pthread_barrier_destroy(&bar));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_barrier_destroy(NULL));
  ASSERT_EQ(EINVAL, pthread_barrier_destroy(&bar));
#endif
#endif
  PASS();
}

TEST test_pthread_barrier_wait(void) {
#if !defined(__APPLE__)
  pthread_barrier_t bar;
  pthread_t th;
  struct barrier_worker_ctx wctx;
  int rc_main;

  ASSERT_EQ(0, pthread_barrier_init(&bar, NULL, 2));
  wctx.bar = &bar;
  wctx.rc = 999;
  ASSERT_EQ(0, pthread_create(&th, NULL, barrier_worker_fn, &wctx));
  rc_main = pthread_barrier_wait(&bar);
  ASSERT_EQ(0, pthread_join(th, NULL));

  ASSERT((rc_main == PTHREAD_BARRIER_SERIAL_THREAD && wctx.rc == 0) ||
         (rc_main == 0 && wctx.rc == PTHREAD_BARRIER_SERIAL_THREAD));
  ASSERT_EQ(0, pthread_barrier_destroy(&bar));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_barrier_wait(NULL));
    bar.ptr = NULL;
    ASSERT_EQ(EINVAL, pthread_barrier_wait(&bar));

    /* Cover spurious wakeup retry in while (phase == b->phase) */
    ASSERT_EQ(0, pthread_barrier_init(&bar, NULL, 2));
    g_spurious_bar = &bar;
    g_spurious_calls = 0;
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.get_proc_address = mock_spurious_get_proc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(0, pthread_barrier_wait(&bar));
    ASSERT_EQ(2, g_spurious_calls);
    posix_pthread_set_win32_hooks(NULL);
    ASSERT_EQ(0, pthread_barrier_destroy(&bar));
  }
#endif
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_barrier) {
  RUN_TEST(test_pthread_barrier_destroy);
  RUN_TEST(test_pthread_barrier_init);
  RUN_TEST(test_pthread_barrier_wait);
}

/* test_pthread_spin.c - Strict C89 Spinlock Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if !defined(__APPLE__)
struct spin_worker_ctx {
  pthread_spinlock_t *sp;
  int rc;
};

static void *spin_contend_worker(void *arg) {
  struct spin_worker_ctx *ctx = (struct spin_worker_ctx *)arg;
  ctx->rc = pthread_spin_trylock(ctx->sp);
  return NULL;
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
static void *spin_oom_malloc(size_t sz) {
  (void)sz;
  return NULL;
}
static unsigned char mock_try_enter_busy(void *cs) {
  (void)cs;
  return 0;
}
#endif

TEST test_pthread_spin_init(void) {
#if !defined(__APPLE__)
  pthread_spinlock_t sp;
  ASSERT_EQ(0, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_destroy(&sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_spin_init(NULL, PTHREAD_PROCESS_PRIVATE));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.malloc_fn = spin_oom_malloc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(ENOMEM, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
#endif
  PASS();
}

TEST test_pthread_spin_destroy(void) {
#if !defined(__APPLE__)
  pthread_spinlock_t sp;
  ASSERT_EQ(0, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_destroy(&sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_spin_destroy(NULL));
  ASSERT_EQ(EINVAL, pthread_spin_destroy(&sp));
#endif
#endif
  PASS();
}

TEST test_pthread_spin_lock(void) {
#if !defined(__APPLE__)
  pthread_spinlock_t sp;
  ASSERT_EQ(0, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_lock(&sp));
  ASSERT_EQ(0, pthread_spin_unlock(&sp));
  ASSERT_EQ(0, pthread_spin_destroy(&sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_spin_lock(NULL));
  ASSERT_EQ(EINVAL, pthread_spin_lock(&sp));
#endif
#endif
  PASS();
}

TEST test_pthread_spin_trylock(void) {
#if !defined(__APPLE__)
  pthread_spinlock_t sp;
  pthread_t th;
  struct spin_worker_ctx ctx;

  ASSERT_EQ(0, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_trylock(&sp));

  /* Contended trylock from a second thread */
  ctx.sp = &sp;
  ctx.rc = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, spin_contend_worker, &ctx));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(EBUSY, ctx.rc);

  ASSERT_EQ(0, pthread_spin_unlock(&sp));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.try_enter_cs = mock_try_enter_busy;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(EBUSY, pthread_spin_trylock(&sp));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, pthread_spin_destroy(&sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_spin_trylock(NULL));
  ASSERT_EQ(EINVAL, pthread_spin_trylock(&sp));
#endif
#endif
  PASS();
}

TEST test_pthread_spin_unlock(void) {
#if !defined(__APPLE__)
  pthread_spinlock_t sp;
  ASSERT_EQ(0, pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_lock(&sp));
  ASSERT_EQ(0, pthread_spin_unlock(&sp));
  ASSERT_EQ(0, pthread_spin_destroy(&sp));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_spin_unlock(NULL));
  ASSERT_EQ(EINVAL, pthread_spin_unlock(&sp));
#endif
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_spin) {
  RUN_TEST(test_pthread_spin_destroy);
  RUN_TEST(test_pthread_spin_init);
  RUN_TEST(test_pthread_spin_lock);
  RUN_TEST(test_pthread_spin_trylock);
  RUN_TEST(test_pthread_spin_unlock);
}

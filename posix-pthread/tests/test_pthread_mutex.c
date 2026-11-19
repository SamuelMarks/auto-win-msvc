/* test_pthread_mutex.c - Strict C89 Mutex Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

struct mutex_contend_ctx {
  pthread_mutex_t *mtx;
  int trylock_rc;
  int timedlock_rc;
};

static void *mutex_contend_worker(void *arg) {
  struct mutex_contend_ctx *ctx = (struct mutex_contend_ctx *)arg;
  ctx->trylock_rc = pthread_mutex_trylock(ctx->mtx);
#if !defined(__APPLE__)
  {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    ctx->timedlock_rc = pthread_mutex_timedlock(ctx->mtx, &ts);
  }
#endif
  return NULL;
}

TEST test_pthread_mutex_init(void) {
  pthread_mutex_t mtx;
  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_init(NULL, NULL));
#endif
  PASS();
}

TEST test_pthread_mutex_destroy(void) {
  pthread_mutex_t mtx;
  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_mutex_lock(void) {
  pthread_mutex_t mtx;
  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_lock(NULL));
#endif
  PASS();
}

TEST test_pthread_mutex_timedlock(void) {
#if !defined(__APPLE__)
  pthread_mutex_t mtx;
  struct timespec ts;
  pthread_t th;
  struct mutex_contend_ctx ctx;

  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_timedlock(&mtx, &ts));

  /* Contended timedlock from another thread returns ETIMEDOUT */
  ctx.mtx = &mtx;
  ctx.trylock_rc = 0;
  ctx.timedlock_rc = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, mutex_contend_worker, &ctx));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(EBUSY, ctx.trylock_rc);
  ASSERT_EQ(ETIMEDOUT, ctx.timedlock_rc);

  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_timedlock(NULL, &ts));
  ASSERT_EQ(EINVAL, pthread_mutex_timedlock(&mtx, NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_mutex_trylock(void) {
  pthread_mutex_t mtx;
  pthread_t th;
  struct mutex_contend_ctx ctx;

  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_trylock(&mtx));

  ctx.mtx = &mtx;
  ctx.trylock_rc = 0;
  ctx.timedlock_rc = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, mutex_contend_worker, &ctx));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(EBUSY, ctx.trylock_rc);

  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_trylock(NULL));
#endif
  PASS();
}

TEST test_pthread_mutex_unlock(void) {
  pthread_mutex_t mtx;
  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_mutex_unlock(NULL));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_mutex) {
  RUN_TEST(test_pthread_mutex_destroy);
  RUN_TEST(test_pthread_mutex_init);
  RUN_TEST(test_pthread_mutex_lock);
  RUN_TEST(test_pthread_mutex_timedlock);
  RUN_TEST(test_pthread_mutex_trylock);
  RUN_TEST(test_pthread_mutex_unlock);
}

/* test_pthread_cond.c - Strict C89 Condition Variable Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
#include <time.h>
/* clang-format on */

struct cond_worker_ctx {
  pthread_mutex_t *mtx;
  pthread_cond_t *cv;
  int ready;
};

static void *cond_signal_worker(void *arg) {
  struct cond_worker_ctx *ctx = (struct cond_worker_ctx *)arg;
  pthread_mutex_lock(ctx->mtx);
  ctx->ready = 1;
  pthread_cond_signal(ctx->cv);
  pthread_mutex_unlock(ctx->mtx);
  return NULL;
}

TEST test_pthread_cond_init(void) {
  pthread_cond_t cv;
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));
  ASSERT_EQ(0, pthread_cond_destroy(&cv));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_cond_init(NULL, NULL));
#endif
  PASS();
}

TEST test_pthread_cond_destroy(void) {
  pthread_cond_t cv;
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));
  ASSERT_EQ(0, pthread_cond_destroy(&cv));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_cond_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_cond_signal(void) {
  pthread_cond_t cv;
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));
  ASSERT_EQ(0, pthread_cond_signal(&cv));
  ASSERT_EQ(0, pthread_cond_destroy(&cv));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_cond_signal(NULL));
#endif
  PASS();
}

TEST test_pthread_cond_broadcast(void) {
  pthread_cond_t cv;
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));
  ASSERT_EQ(0, pthread_cond_broadcast(&cv));
  ASSERT_EQ(0, pthread_cond_destroy(&cv));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_cond_broadcast(NULL));
#endif
  PASS();
}

TEST test_pthread_cond_timedwait(void) {
  pthread_mutex_t mtx;
  pthread_cond_t cv;
  struct timespec ts;
  int rc;

  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_cond_timedwait(NULL, &mtx, &ts));
  ASSERT_EQ(EINVAL, pthread_cond_timedwait(&cv, NULL, &ts));
  ASSERT_EQ(EINVAL, pthread_cond_timedwait(&cv, &mtx, NULL));

  ts.tv_sec = 0;
  ts.tv_nsec = 1000000L;
  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  rc = pthread_cond_timedwait(&cv, &mtx, &ts);
  ASSERT_EQ(ETIMEDOUT, rc);
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));

  {
    pthread_t th;
    struct cond_worker_ctx wctx;
    wctx.mtx = &mtx;
    wctx.cv = &cv;
    wctx.ready = 0;
    ts.tv_sec = 5;
    ts.tv_nsec = 0;
    ASSERT_EQ(0, pthread_mutex_lock(&mtx));
    ASSERT_EQ(0, pthread_create(&th, NULL, cond_signal_worker, &wctx));
    while (!wctx.ready) {
      rc = pthread_cond_timedwait(&cv, &mtx, &ts);
      ASSERT_EQ(0, rc);
    }
    ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
    ASSERT_EQ(0, pthread_join(th, NULL));
  }
#else
  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  rc = pthread_cond_timedwait(&cv, &mtx, &ts);
  (void)rc;
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
#endif

  ASSERT_EQ(0, pthread_cond_destroy(&cv));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
  PASS();
}

TEST test_pthread_cond_wait(void) {
  pthread_mutex_t mtx;
  pthread_cond_t cv;
  pthread_t th;
  struct cond_worker_ctx wctx;

  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_cond_wait(NULL, &mtx));
  ASSERT_EQ(EINVAL, pthread_cond_wait(&cv, NULL));
#endif

  wctx.mtx = &mtx;
  wctx.cv = &cv;
  wctx.ready = 0;

  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  ASSERT_EQ(0, pthread_create(&th, NULL, cond_signal_worker, &wctx));
  while (!wctx.ready) {
    ASSERT_EQ(0, pthread_cond_wait(&cv, &mtx));
  }
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));
  ASSERT_EQ(0, pthread_join(th, NULL));

  ASSERT_EQ(0, pthread_cond_destroy(&cv));
  ASSERT_EQ(0, pthread_mutex_destroy(&mtx));
  PASS();
}

SUITE(suite_posix_pthread_pthread_cond) {
  RUN_TEST(test_pthread_cond_broadcast);
  RUN_TEST(test_pthread_cond_destroy);
  RUN_TEST(test_pthread_cond_init);
  RUN_TEST(test_pthread_cond_signal);
  RUN_TEST(test_pthread_cond_timedwait);
  RUN_TEST(test_pthread_cond_wait);
}

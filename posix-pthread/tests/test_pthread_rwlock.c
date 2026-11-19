/* test_pthread_rwlock.c - Strict C89 Read-Write Lock Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

struct rw_contend_ctx {
  pthread_rwlock_t *rw;
  int tryrd_rc;
  int trywr_rc;
  int timedrd_rc;
  int timedwr_rc;
};

static void *rw_contend_worker(void *arg) {
  struct rw_contend_ctx *ctx = (struct rw_contend_ctx *)arg;
  ctx->tryrd_rc = pthread_rwlock_tryrdlock(ctx->rw);
  ctx->trywr_rc = pthread_rwlock_trywrlock(ctx->rw);
#if !defined(__APPLE__)
  {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    ctx->timedrd_rc = pthread_rwlock_timedrdlock(ctx->rw, &ts);
    ctx->timedwr_rc = pthread_rwlock_timedwrlock(ctx->rw, &ts);
  }
#endif
  return NULL;
}

TEST test_pthread_rwlock_init(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_init(NULL, NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_destroy(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_rwlock_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_rdlock(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_rdlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_rdlock(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_wrlock(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_wrlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_wrlock(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_tryrdlock(void) {
  pthread_rwlock_t rw;
  pthread_t th;
  struct rw_contend_ctx ctx;

  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_tryrdlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));

  /* While write-locked, another thread's tryrdlock returns EBUSY */
  ASSERT_EQ(0, pthread_rwlock_wrlock(&rw));
  ctx.rw = &rw;
  ctx.tryrd_rc = 0;
  ctx.trywr_rc = 0;
  ctx.timedrd_rc = 0;
  ctx.timedwr_rc = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, rw_contend_worker, &ctx));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(EBUSY, ctx.tryrd_rc);
  ASSERT_EQ(EBUSY, ctx.trywr_rc);
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_tryrdlock(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_trywrlock(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_trywrlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_trywrlock(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlock_timedrdlock(void) {
#if !defined(__APPLE__)
  pthread_rwlock_t rw;
  struct timespec ts;
  pthread_t th;
  struct rw_contend_ctx ctx;

  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_timedrdlock(&rw, &ts));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));

  /* Contended timedrdlock & timedwrlock from worker */
  ASSERT_EQ(0, pthread_rwlock_wrlock(&rw));
  ctx.rw = &rw;
  ctx.tryrd_rc = 0;
  ctx.trywr_rc = 0;
  ctx.timedrd_rc = 0;
  ctx.timedwr_rc = 0;
  ASSERT_EQ(0, pthread_create(&th, NULL, rw_contend_worker, &ctx));
  ASSERT_EQ(0, pthread_join(th, NULL));
  ASSERT_EQ(ETIMEDOUT, ctx.timedrd_rc);
  ASSERT_EQ(ETIMEDOUT, ctx.timedwr_rc);
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_timedrdlock(NULL, &ts));
  ASSERT_EQ(EINVAL, pthread_rwlock_timedrdlock(&rw, NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_rwlock_timedwrlock(void) {
#if !defined(__APPLE__)
  pthread_rwlock_t rw;
  struct timespec ts;

  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_timedwrlock(&rw, &ts));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_rwlock_timedwrlock(NULL, &ts));
  ASSERT_EQ(EINVAL, pthread_rwlock_timedwrlock(&rw, NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_rwlock_unlock(void) {
  pthread_rwlock_t rw;
  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_wrlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_destroy(&rw));
#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    pthread_rwlock_t many[33];
    int i;
    ASSERT_EQ(EINVAL, pthread_rwlock_unlock(NULL));
    for (i = 0; i < 33; ++i) {
      ASSERT_EQ(0, pthread_rwlock_init(&many[i], NULL));
      ASSERT_EQ(0, pthread_rwlock_wrlock(&many[i]));
    }
    /* Slot 32 overflowed the 32-entry table; release its underlying SRWLock
       cleanly via mock-null unlock, then unlock slots 0..31 normally */
    many[32].p = NULL;
    for (i = 31; i >= 0; --i) {
      ASSERT_EQ(0, pthread_rwlock_unlock(&many[i]));
      ASSERT_EQ(0, pthread_rwlock_destroy(&many[i]));
    }
    ASSERT_EQ(0, pthread_rwlock_destroy(&many[32]));
  }
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_rwlock) {
  RUN_TEST(test_pthread_rwlock_destroy);
  RUN_TEST(test_pthread_rwlock_init);
  RUN_TEST(test_pthread_rwlock_rdlock);
  RUN_TEST(test_pthread_rwlock_timedrdlock);
  RUN_TEST(test_pthread_rwlock_timedwrlock);
  RUN_TEST(test_pthread_rwlock_tryrdlock);
  RUN_TEST(test_pthread_rwlock_trywrlock);
  RUN_TEST(test_pthread_rwlock_unlock);
  RUN_TEST(test_pthread_rwlock_wrlock);
}

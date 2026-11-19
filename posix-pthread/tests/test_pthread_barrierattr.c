/* test_pthread_barrierattr.c - Strict C89 Barrier Attribute Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_pthread_barrierattr_init(void) {
#if !defined(__APPLE__)
  pthread_barrierattr_t attr;
  ASSERT_EQ(0, pthread_barrierattr_init(&attr));
  ASSERT_EQ(0, pthread_barrierattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_barrierattr_init(NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_barrierattr_destroy(void) {
#if !defined(__APPLE__)
  pthread_barrierattr_t attr;
  ASSERT_EQ(0, pthread_barrierattr_init(&attr));
  ASSERT_EQ(0, pthread_barrierattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_barrierattr_destroy(NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_barrierattr_getpshared(void) {
#if !defined(__APPLE__)
  pthread_barrierattr_t attr;
  int pshared = -1;
  ASSERT_EQ(0, pthread_barrierattr_init(&attr));
  ASSERT_EQ(0, pthread_barrierattr_getpshared(&attr, &pshared));
  ASSERT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
  ASSERT_EQ(0, pthread_barrierattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_barrierattr_getpshared(NULL, &pshared));
  ASSERT_EQ(EINVAL, pthread_barrierattr_getpshared(&attr, NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_barrierattr_setpshared(void) {
#if !defined(__APPLE__)
  pthread_barrierattr_t attr;
  ASSERT_EQ(0, pthread_barrierattr_init(&attr));
  ASSERT_EQ(0, pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_barrierattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL,
            pthread_barrierattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(EINVAL,
            pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
#endif
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_barrierattr) {
  RUN_TEST(test_pthread_barrierattr_destroy);
  RUN_TEST(test_pthread_barrierattr_getpshared);
  RUN_TEST(test_pthread_barrierattr_init);
  RUN_TEST(test_pthread_barrierattr_setpshared);
}

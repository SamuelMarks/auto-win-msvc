/* test_pthread_rwlockattr.c - Strict C89 Read-Write Lock Attribute Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_pthread_rwlockattr_init(void) {
  pthread_rwlockattr_t attr;
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_rwlockattr_init(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlockattr_destroy(void) {
  pthread_rwlockattr_t attr;
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_rwlockattr_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_rwlockattr_getpshared(void) {
  pthread_rwlockattr_t attr;
  int pshared = -1;
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_getpshared(&attr, &pshared));
  ASSERT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_rwlockattr_getpshared(NULL, &pshared));
  ASSERT_EQ(-1, pthread_rwlockattr_getpshared(&attr, NULL));
#endif
  PASS();
}

TEST test_pthread_rwlockattr_setpshared(void) {
  pthread_rwlockattr_t attr;
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_rwlockattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(-1, pthread_rwlockattr_setpshared(&attr, 999));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_rwlockattr) {
  RUN_TEST(test_pthread_rwlockattr_destroy);
  RUN_TEST(test_pthread_rwlockattr_getpshared);
  RUN_TEST(test_pthread_rwlockattr_init);
  RUN_TEST(test_pthread_rwlockattr_setpshared);
}

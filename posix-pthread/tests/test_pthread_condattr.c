/* test_pthread_condattr.c - Strict C89 Condition Variable Attribute Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_pthread_condattr_init(void) {
  pthread_condattr_t attr;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_init(NULL));
#endif
  PASS();
}

TEST test_pthread_condattr_destroy(void) {
  pthread_condattr_t attr;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_condattr_getclock(void) {
#if !defined(__APPLE__)
  pthread_condattr_t attr;
  clockid_t cid = 0;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_getclock(&attr, &cid));
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_getclock(NULL, &cid));
  ASSERT_EQ(-1, pthread_condattr_getclock(&attr, NULL));
#endif
#endif
  PASS();
}

TEST test_pthread_condattr_setclock(void) {
#if !defined(__APPLE__)
  pthread_condattr_t attr;
  clockid_t cid = 0;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_getclock(&attr, &cid));
  ASSERT_EQ(0, pthread_condattr_setclock(&attr, cid));
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_setclock(NULL, 0));
#endif
#endif
  PASS();
}

TEST test_pthread_condattr_getpshared(void) {
  pthread_condattr_t attr;
  int pshared = -1;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_getpshared(&attr, &pshared));
  ASSERT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_getpshared(NULL, &pshared));
  ASSERT_EQ(-1, pthread_condattr_getpshared(&attr, NULL));
#endif
  PASS();
}

TEST test_pthread_condattr_setpshared(void) {
  pthread_condattr_t attr;
  ASSERT_EQ(0, pthread_condattr_init(&attr));
  ASSERT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
#if !defined(__CYGWIN__) && !defined(__EMSCRIPTEN__)
  ASSERT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
#endif
  ASSERT_EQ(0, pthread_condattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_condattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(-1, pthread_condattr_setpshared(&attr, 999));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_condattr) {
  RUN_TEST(test_pthread_condattr_destroy);
  RUN_TEST(test_pthread_condattr_getclock);
  RUN_TEST(test_pthread_condattr_getpshared);
  RUN_TEST(test_pthread_condattr_init);
  RUN_TEST(test_pthread_condattr_setclock);
  RUN_TEST(test_pthread_condattr_setpshared);
}

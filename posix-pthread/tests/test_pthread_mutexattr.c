/* test_pthread_mutexattr.c - Strict C89 Mutex Attribute Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_pthread_mutexattr_init(void) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_init(NULL));
#endif
  PASS();
}

TEST test_pthread_mutexattr_destroy(void) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_destroy(NULL));
#endif
  PASS();
}

TEST test_pthread_mutexattr_getprioceiling(void) {
  pthread_mutexattr_t attr;
  int ceil_val = 0;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(0, pthread_mutexattr_getprioceiling(&attr, &ceil_val));
  ASSERT_EQ(-1, pthread_mutexattr_getprioceiling(NULL, &ceil_val));
  ASSERT_EQ(-1, pthread_mutexattr_getprioceiling(&attr, NULL));
#else
  (void)pthread_mutexattr_getprioceiling(&attr, &ceil_val);
#endif
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  PASS();
}

TEST test_pthread_mutexattr_setprioceiling(void) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(0, pthread_mutexattr_setprioceiling(&attr, 1));
  ASSERT_EQ(-1, pthread_mutexattr_setprioceiling(NULL, 1));
#endif
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  PASS();
}

TEST test_pthread_mutexattr_getprotocol(void) {
  pthread_mutexattr_t attr;
  int proto = -1;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_getprotocol(&attr, &proto));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_getprotocol(NULL, &proto));
  ASSERT_EQ(-1, pthread_mutexattr_getprotocol(&attr, NULL));
#endif
  PASS();
}

TEST test_pthread_mutexattr_setprotocol(void) {
  pthread_mutexattr_t attr;
  int proto = 0;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_getprotocol(&attr, &proto));
  ASSERT_EQ(0, pthread_mutexattr_setprotocol(&attr, proto));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_setprotocol(NULL, 0));
#endif
  PASS();
}

TEST test_pthread_mutexattr_getpshared(void) {
  pthread_mutexattr_t attr;
  int pshared = -1;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_getpshared(&attr, &pshared));
  ASSERT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_getpshared(NULL, &pshared));
  ASSERT_EQ(-1, pthread_mutexattr_getpshared(&attr, NULL));
#endif
  PASS();
}

TEST test_pthread_mutexattr_setpshared(void) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(-1, pthread_mutexattr_setpshared(&attr, 999));
#endif
  PASS();
}

TEST test_pthread_mutexattr_gettype(void) {
  pthread_mutexattr_t attr;
  int mtype = -1;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_gettype(&attr, &mtype));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_gettype(NULL, &mtype));
  ASSERT_EQ(-1, pthread_mutexattr_gettype(&attr, NULL));
#endif
  PASS();
}

TEST test_pthread_mutexattr_settype(void) {
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, pthread_mutexattr_settype(NULL, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(-1, pthread_mutexattr_settype(&attr, 999));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_mutexattr) {
  RUN_TEST(test_pthread_mutexattr_destroy);
  RUN_TEST(test_pthread_mutexattr_getprioceiling);
  RUN_TEST(test_pthread_mutexattr_getprotocol);
  RUN_TEST(test_pthread_mutexattr_getpshared);
  RUN_TEST(test_pthread_mutexattr_gettype);
  RUN_TEST(test_pthread_mutexattr_init);
  RUN_TEST(test_pthread_mutexattr_setprioceiling);
  RUN_TEST(test_pthread_mutexattr_setprotocol);
  RUN_TEST(test_pthread_mutexattr_setpshared);
  RUN_TEST(test_pthread_mutexattr_settype);
}

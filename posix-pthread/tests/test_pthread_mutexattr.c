#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_mutexattr_destroy(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_destroy */
}

TEST test_pthread_mutexattr_getprioceiling(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_getprioceiling */
}

TEST test_pthread_mutexattr_getprotocol(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_getprotocol */
}

TEST test_pthread_mutexattr_getpshared(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_getpshared */
}

TEST test_pthread_mutexattr_gettype(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_gettype */
}

TEST test_pthread_mutexattr_init(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_init */
}

TEST test_pthread_mutexattr_setprioceiling(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_setprioceiling */
}

TEST test_pthread_mutexattr_setprotocol(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_setprotocol */
}

TEST test_pthread_mutexattr_setpshared(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_setpshared */
}

TEST test_pthread_mutexattr_settype(void) {
  SKIP(); /* Generated stub for pthread_mutexattr_settype */
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

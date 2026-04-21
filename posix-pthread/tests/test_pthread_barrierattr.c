#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_barrierattr_destroy(void) {
  SKIP(); /* Generated stub for pthread_barrierattr_destroy */
}

TEST test_pthread_barrierattr_getpshared(void) {
  SKIP(); /* Generated stub for pthread_barrierattr_getpshared */
}

TEST test_pthread_barrierattr_init(void) {
  SKIP(); /* Generated stub for pthread_barrierattr_init */
}

TEST test_pthread_barrierattr_setpshared(void) {
  SKIP(); /* Generated stub for pthread_barrierattr_setpshared */
}

SUITE(suite_posix_pthread_pthread_barrierattr) {
  RUN_TEST(test_pthread_barrierattr_destroy);
  RUN_TEST(test_pthread_barrierattr_getpshared);
  RUN_TEST(test_pthread_barrierattr_init);
  RUN_TEST(test_pthread_barrierattr_setpshared);
}

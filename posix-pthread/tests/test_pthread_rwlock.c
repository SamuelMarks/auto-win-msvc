#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_rwlock_destroy(void) {
  SKIP(); /* Generated stub for pthread_rwlock_destroy */
}

TEST test_pthread_rwlock_init(void) {
  SKIP(); /* Generated stub for pthread_rwlock_init */
}

TEST test_pthread_rwlock_rdlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_rdlock */
}

TEST test_pthread_rwlock_timedrdlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_timedrdlock */
}

TEST test_pthread_rwlock_timedwrlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_timedwrlock */
}

TEST test_pthread_rwlock_tryrdlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_tryrdlock */
}

TEST test_pthread_rwlock_trywrlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_trywrlock */
}

TEST test_pthread_rwlock_unlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_unlock */
}

TEST test_pthread_rwlock_wrlock(void) {
  SKIP(); /* Generated stub for pthread_rwlock_wrlock */
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

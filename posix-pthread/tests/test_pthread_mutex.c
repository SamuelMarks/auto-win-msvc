#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_mutex_destroy(void) {
  SKIP(); /* Generated stub for pthread_mutex_destroy */
}

TEST test_pthread_mutex_init(void) {
  SKIP(); /* Generated stub for pthread_mutex_init */
}

TEST test_pthread_mutex_lock(void) {
  SKIP(); /* Generated stub for pthread_mutex_lock */
}

TEST test_pthread_mutex_timedlock(void) {
  SKIP(); /* Generated stub for pthread_mutex_timedlock */
}

TEST test_pthread_mutex_trylock(void) {
  SKIP(); /* Generated stub for pthread_mutex_trylock */
}

TEST test_pthread_mutex_unlock(void) {
  SKIP(); /* Generated stub for pthread_mutex_unlock */
}

SUITE(suite_posix_pthread_pthread_mutex) {
  RUN_TEST(test_pthread_mutex_destroy);
  RUN_TEST(test_pthread_mutex_init);
  RUN_TEST(test_pthread_mutex_lock);
  RUN_TEST(test_pthread_mutex_timedlock);
  RUN_TEST(test_pthread_mutex_trylock);
  RUN_TEST(test_pthread_mutex_unlock);
}

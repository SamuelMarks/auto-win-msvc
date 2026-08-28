#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_spin_destroy(void) {
  SKIP(); /* Generated stub for pthread_spin_destroy */
}

TEST test_pthread_spin_init(void) {
  SKIP(); /* Generated stub for pthread_spin_init */
}

TEST test_pthread_spin_lock(void) {
  SKIP(); /* Generated stub for pthread_spin_lock */
}

TEST test_pthread_spin_trylock(void) {
  SKIP(); /* Generated stub for pthread_spin_trylock */
}

TEST test_pthread_spin_unlock(void) {
  SKIP(); /* Generated stub for pthread_spin_unlock */
}

SUITE(suite_posix_pthread_pthread_spin) {
  RUN_TEST(test_pthread_spin_destroy);
  RUN_TEST(test_pthread_spin_init);
  RUN_TEST(test_pthread_spin_lock);
  RUN_TEST(test_pthread_spin_trylock);
  RUN_TEST(test_pthread_spin_unlock);
}

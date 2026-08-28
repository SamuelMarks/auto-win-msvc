#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_barrier_destroy(void) {
  SKIP(); /* Generated stub for pthread_barrier_destroy */
}

TEST test_pthread_barrier_init(void) {
  SKIP(); /* Generated stub for pthread_barrier_init */
}

TEST test_pthread_barrier_wait(void) {
  SKIP(); /* Generated stub for pthread_barrier_wait */
}

SUITE(suite_posix_pthread_pthread_barrier) {
  RUN_TEST(test_pthread_barrier_destroy);
  RUN_TEST(test_pthread_barrier_init);
  RUN_TEST(test_pthread_barrier_wait);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_cond_broadcast(void) {
  SKIP(); /* Generated stub for pthread_cond_broadcast */
}

TEST test_pthread_cond_destroy(void) {
  SKIP(); /* Generated stub for pthread_cond_destroy */
}

TEST test_pthread_cond_init(void) {
  SKIP(); /* Generated stub for pthread_cond_init */
}

TEST test_pthread_cond_signal(void) {
  SKIP(); /* Generated stub for pthread_cond_signal */
}

TEST test_pthread_cond_timedwait(void) {
  SKIP(); /* Generated stub for pthread_cond_timedwait */
}

TEST test_pthread_cond_wait(void) {
  SKIP(); /* Generated stub for pthread_cond_wait */
}

SUITE(suite_posix_pthread_pthread_cond) {
  RUN_TEST(test_pthread_cond_broadcast);
  RUN_TEST(test_pthread_cond_destroy);
  RUN_TEST(test_pthread_cond_init);
  RUN_TEST(test_pthread_cond_signal);
  RUN_TEST(test_pthread_cond_timedwait);
  RUN_TEST(test_pthread_cond_wait);
}

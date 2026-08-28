#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test__posix_pthread_cleanup_push(void) {
  SKIP(); /* Generated stub for _posix_pthread_cleanup_push */
}

TEST test__posix_pthread_cleanup_pop(void) {
  SKIP(); /* Generated stub for _posix_pthread_cleanup_pop */
}

SUITE(suite_posix_pthread_) {
  RUN_TEST(test__posix_pthread_cleanup_push);
  RUN_TEST(test__posix_pthread_cleanup_pop);
}

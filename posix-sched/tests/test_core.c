#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sched.h"
/* clang-format on */

TEST test_sched(void) {
  if (sched_yield() == -1)
    SKIP();
  PASS();
}

TEST test_posix_sched_yield(void) {
  SKIP(); /* Generated stub for posix_sched_yield */
}

TEST test_posix_sched_setaffinity(void) {
  SKIP(); /* Generated stub for posix_sched_setaffinity */
}

TEST test_posix_sched_getaffinity(void) {
  SKIP(); /* Generated stub for posix_sched_getaffinity */
}

SUITE(suite_posix_sched_core) {
  RUN_TEST(test_sched);
  RUN_TEST(test_posix_sched_yield);
  RUN_TEST(test_posix_sched_setaffinity);
  RUN_TEST(test_posix_sched_getaffinity);
}

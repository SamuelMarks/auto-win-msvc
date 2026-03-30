#include "greatest.h"
#include "posix-sched.h"

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_sched_yield(void) {
  ASSERT_EQ_FMT(0, posix_sched_yield(), "%d");
  PASS();
}

TEST test_sched_affinity(void) {
  cpu_set_t mask;
  ASSERT_EQ_FMT(0, posix_sched_getaffinity(0, sizeof(cpu_set_t), &mask), "%d");
  ASSERT(mask.bits != 0);

  /* We don't want to actually restrict our thread, just check setting the same
   * mask */
  ASSERT_EQ_FMT(0, posix_sched_setaffinity(0, sizeof(cpu_set_t), &mask), "%d");

  PASS();
}

SUITE(sched_suite) {
  RUN_TEST(test_sched_yield);
  RUN_TEST(test_sched_affinity);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(sched_suite);
  GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif

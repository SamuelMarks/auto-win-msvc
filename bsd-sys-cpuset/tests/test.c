/* clang-format off */
#include "bsd-sys-cpuset.h"
#include "greatest.h"
#include <errno.h>
/* clang-format on */

TEST test_macros(void) {
#if defined(_MSC_VER) || defined(__MINGW32__)
  cpuset_t set;
  CPU_ZERO(&set);
  ASSERT_EQ(0, set.__bits[0]);

  CPU_SET(0, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));

  CPU_SET(1, &set);
  ASSERT_EQ(1, CPU_ISSET(1, &set));

  CPU_CLR(0, &set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(1, CPU_ISSET(1, &set));
#endif
  PASS();
}

TEST test_getaffinity(void) {
#if defined(_MSC_VER) || defined(__MINGW32__)
  cpuset_t mask;
  int ret;

  /* Null mask */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* Invalid size */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, 0, &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* Valid process */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);
  ASSERT(mask.__bits[0] != 0);

  /* Valid thread */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);
  ASSERT(mask.__bits[0] != 0);

  /* Valid ROOT */
  ret = cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);
  ASSERT(mask.__bits[0] != 0);

  /* Valid CPUSET with JAIL */
  ret = cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_JAIL, -1,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(0, ret);
  ASSERT(mask.__bits[0] != 0);

  /* Valid CPUSET with IRQ */
  ret = cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_IRQ, -1,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(0, ret);
  ASSERT(mask.__bits[0] != 0);

  /* Invalid level/which */
  ret = cpuset_getaffinity(999, 999, -1, sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ENOSYS, errno);

  /* Invalid ID */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, 9999999,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ESRCH, errno);

  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 9999999,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ESRCH, errno);
#endif
  PASS();
}

TEST test_setaffinity(void) {
#if defined(_MSC_VER) || defined(__MINGW32__)
  cpuset_t mask;
  int ret;

  /* First get current affinity so we can restore it */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);

  /* Set it to itself */
  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);

  /* Set thread affinity to itself */
  ret = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);

  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(0, ret);

  /* Set CPUSET/JAIL to itself */
  ret = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_JAIL, -1,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(0, ret);

  /* Set ROOT should fail with EPERM */
  ret = cpuset_setaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EPERM, errno);

  /* Invalid mask/size */
  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, 0, &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* Invalid level/which */
  ret = cpuset_setaffinity(999, 999, -1, sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ENOSYS, errno);

  /* Invalid ID */
  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, 9999999,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ESRCH, errno);

  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 9999999,
                           sizeof(cpuset_t), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ESRCH, errno);

  /* Invalid mask bit (0 is not allowed for affinity) */
  CPU_ZERO(&mask);
  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  ret = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(cpuset_t),
                           &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_macros);
  RUN_TEST(test_getaffinity);
  RUN_TEST(test_setaffinity);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

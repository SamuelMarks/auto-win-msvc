#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sched.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_posix_sched_get_info(void) {
  enum posix_sched_error_code rc;
  int info;

  info = 0;
  rc = posix_sched_get_info(NULL);
  if (rc != POSIX_SCHED_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SCHED_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sched_get_info(&info);
  if (rc != POSIX_SCHED_SUCCESS) {
    printf("posix_sched_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_cpu_set_macros(void) {
  cpu_set_t set;
  int cpu_oob;

  cpu_oob = 64;
  CPU_ZERO(&set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));
  ASSERT_EQ(0, CPU_ISSET(cpu_oob, &set));

  CPU_SET(0, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));

  CPU_SET(63, &set);
  ASSERT_EQ(1, CPU_ISSET(63, &set));

  CPU_SET(cpu_oob, &set);
  ASSERT_EQ(0, CPU_ISSET(cpu_oob, &set));

  CPU_CLR(0, &set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(1, CPU_ISSET(63, &set));

  CPU_CLR(cpu_oob, &set);

  PASS();
}

TEST test_posix_sched_yield(void) {
  int res;

  res = posix_sched_yield();
  ASSERT_EQ(0, res);

  res = sched_yield();
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_sched_affinity(void) {
  cpu_set_t set;
  int res;

  CPU_ZERO(&set);
  CPU_SET(0, &set);

  /* Error: NULL mask */
  errno = 0;
  res = posix_sched_getaffinity(0, sizeof(cpu_set_t), NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  errno = 0;
  res = posix_sched_setaffinity(0, sizeof(cpu_set_t), NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  /* Error: invalid cpusetsize */
  errno = 0;
  res = posix_sched_getaffinity(0, 0, &set);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  errno = 0;
  res = posix_sched_setaffinity(0, 0, &set);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

#if defined(_WIN32) || defined(_MSC_VER)
  /* Error: invalid pid */
  errno = 0;
  res = posix_sched_getaffinity(99999999, sizeof(cpu_set_t), &set);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(ESRCH, errno);

  errno = 0;
  res = posix_sched_setaffinity(99999999, sizeof(cpu_set_t), &set);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(ESRCH, errno);
#endif

  /* Success: current process */
  res = posix_sched_getaffinity(0, sizeof(cpu_set_t), &set);
  ASSERT_EQ(0, res);

  res = posix_sched_setaffinity(0, sizeof(cpu_set_t), &set);
  ASSERT_EQ(0, res);

  /* Test macro aliases */
  res = sched_getaffinity(0, sizeof(cpu_set_t), &set);
  ASSERT_EQ(0, res);

  res = sched_setaffinity(0, sizeof(cpu_set_t), &set);
  ASSERT_EQ(0, res);

  PASS();
}

SUITE(suite_posix_sched_core) {
  RUN_TEST(test_posix_sched_get_info);
  RUN_TEST(test_cpu_set_macros);
  RUN_TEST(test_posix_sched_yield);
  RUN_TEST(test_posix_sched_affinity);
}

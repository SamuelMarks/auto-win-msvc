#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-cpuset.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_cpuset_init(void) {
  enum bsd_sys_cpuset_error_code rc;
  int status;

  status = 0;
  rc = bsd_sys_cpuset_init(NULL);
  if (rc != BSD_SYS_CPUSET_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_CPUSET_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_cpuset_init(&status);
  if (rc != BSD_SYS_CPUSET_SUCCESS) {
    printf("bsd_sys_cpuset_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_cpuset_macros(void) {
  cpuset_t set;

  CPU_ZERO(&set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));

  CPU_SET(0, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));

  CPU_SET(1, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set));
  ASSERT_EQ(1, CPU_ISSET(1, &set));

  CPU_CLR(0, &set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(1, CPU_ISSET(1, &set));

  PASS();
}

TEST test_cpuset_getaffinity(void) {
  cpuset_t mask;
  error_type_t err;

  /* NULL mask */
  err =
      cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), NULL);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  /* Invalid size */
  err = cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, 0, &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  /* Unknown level */
  err = cpuset_getaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(ENOSYS, errno);

  /* CPU_LEVEL_ROOT */
  err =
      cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  ASSERT_EQ(ERR_NONE, err);

  /* CPU_LEVEL_CPUSET with CPU_WHICH_PID */
  err = cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  ASSERT_EQ(ERR_NONE, err);

  /* CPU_LEVEL_WHICH with CPU_WHICH_TID */
  err = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);
  ASSERT_EQ(ERR_NONE, err);

  PASS();
}

TEST test_cpuset_setaffinity(void) {
  cpuset_t mask;
  error_type_t err;

  /* NULL mask */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           NULL);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  /* Invalid size */
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, 0, &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  /* Empty mask (0) */
  CPU_ZERO(&mask);
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  /* CPU_LEVEL_ROOT not permitted to change */
  CPU_SET(0, &mask);
  err =
      cpuset_setaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EPERM, errno);

  /* Unknown level */
  err = cpuset_setaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(ENOSYS, errno);

  /* CPU_LEVEL_CPUSET with CPU_WHICH_PID */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  ASSERT_EQ(ERR_NONE, err);

  /* CPU_LEVEL_WHICH with CPU_WHICH_TID */
  err = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);
  ASSERT_EQ(ERR_NONE, err);

  PASS();
}

SUITE(suite_bsd_sys_cpuset_core) {
  RUN_TEST(test_bsd_sys_cpuset_init);
  RUN_TEST(test_cpuset_macros);
  RUN_TEST(test_cpuset_getaffinity);
  RUN_TEST(test_cpuset_setaffinity);
}

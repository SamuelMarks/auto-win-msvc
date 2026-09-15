#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-resource.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_posix_sys_resource_get_info(void) {
  enum posix_sys_resource_error_code rc;
  int info;

  info = 0;
  rc = posix_sys_resource_get_info(NULL);
  if (rc != POSIX_SYS_RESOURCE_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SYS_RESOURCE_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sys_resource_get_info(&info);
  if (rc != POSIX_SYS_RESOURCE_SUCCESS) {
    printf("posix_sys_resource_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_getrusage(void) {
  struct rusage usage;
  int res;

  errno = 0;
  res = posix_getrusage(RUSAGE_SELF, NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EFAULT, errno);

#if defined(_WIN32) || defined(_MSC_VER)
  errno = 0;
  res = posix_getrusage(RUSAGE_CHILDREN, &usage);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);
#endif

  res = posix_getrusage(RUSAGE_SELF, &usage);
  ASSERT_EQ(0, res);

  res = getrusage(RUSAGE_SELF, &usage);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_getrlimit(void) {
  struct rlimit r;
  int res;

  errno = 0;
  res = posix_getrlimit(RLIMIT_NOFILE, NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EFAULT, errno);

  res = posix_getrlimit(RLIMIT_NOFILE, &r);
  ASSERT_EQ(0, res);
  ASSERT(r.rlim_cur > 0);

  res = posix_getrlimit(RLIMIT_CPU, &r);
  ASSERT_EQ(0, res);

  res = getrlimit(RLIMIT_NOFILE, &r);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_setrlimit(void) {
  struct rlimit r;
  int res;

  errno = 0;
  res = posix_setrlimit(RLIMIT_NOFILE, NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EFAULT, errno);

  if (posix_getrlimit(RLIMIT_NOFILE, &r) == 0) {
    res = posix_setrlimit(RLIMIT_NOFILE, &r);
    ASSERT_EQ(0, res);
  }

#if defined(_WIN32) || defined(_MSC_VER)
  r.rlim_cur = 9000;
  r.rlim_max = 9000;
  res = posix_setrlimit(RLIMIT_NOFILE, &r);
  ASSERT_EQ(0, res);

  res = posix_setrlimit(RLIMIT_CPU, &r);
  ASSERT_EQ(0, res);
#endif

  if (getrlimit(RLIMIT_NOFILE, &r) == 0) {
    res = setrlimit(RLIMIT_NOFILE, &r);
    ASSERT_EQ(0, res);
  }

  PASS();
}

SUITE(suite_posix_sys_resource_core) {
  RUN_TEST(test_posix_sys_resource_get_info);
  RUN_TEST(test_posix_getrusage);
  RUN_TEST(test_posix_getrlimit);
  RUN_TEST(test_posix_setrlimit);
}

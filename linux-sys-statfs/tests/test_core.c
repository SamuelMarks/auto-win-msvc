#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-statfs.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_statfs_init(void) {
  enum linux_sys_statfs_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_statfs_init(NULL);
  if (rc != LINUX_SYS_STATFS_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_STATFS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_statfs_init(&status);
  if (rc != LINUX_SYS_STATFS_SUCCESS) {
    printf("linux_sys_statfs_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_statfs_functions(void) {
#if defined(_MSC_VER)
  struct statfs s;
  int ret;

  /* Invalid calls */
  ret = statfs(NULL, &s);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EFAULT, errno);

  ret = statfs("C:", NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EFAULT, errno);

  ret = fstatfs(-1, &s);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EBADF, errno);

  ret = fstatfs(0, NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EFAULT, errno);

  /* Valid call on current directory */
  ret = statfs(".", &s);
  if (ret == 0) {
    ASSERT(s.f_bsize > 0);
  }
#endif
  PASS();
}

SUITE(suite_linux_sys_statfs_core) {
  RUN_TEST(test_linux_sys_statfs_init);
  RUN_TEST(test_statfs_functions);
}

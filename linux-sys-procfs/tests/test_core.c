#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-procfs.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_procfs_init(void) {
  enum linux_sys_procfs_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_procfs_init(NULL);
  if (rc != LINUX_SYS_PROCFS_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_PROCFS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_procfs_init(&status);
  if (rc != LINUX_SYS_PROCFS_SUCCESS) {
    printf("linux_sys_procfs_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_linux_sys_procfs_core) { RUN_TEST(test_linux_sys_procfs_init); }

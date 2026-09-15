#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-user.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_user_init(void) {
  enum linux_sys_user_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_user_init(NULL);
  if (rc != LINUX_SYS_USER_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_USER_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_user_init(&status);
  if (rc != LINUX_SYS_USER_SUCCESS) {
    printf("linux_sys_user_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_linux_sys_user_core) { RUN_TEST(test_linux_sys_user_init); }

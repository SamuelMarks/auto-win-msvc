#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-user.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_user_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;
  rc = linux_sys_user_init(NULL);

  rc = linux_sys_user_init(&status);

  PASS();
}

SUITE(suite_linux_sys_user_core) { RUN_TEST(test_linux_sys_user_init); }

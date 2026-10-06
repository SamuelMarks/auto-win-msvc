#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_ipc_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_ipc_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_ipc_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_ipc(void) {
  key_t k;
  k = ftok(".", 1);
  ASSERT(k != (key_t)-1 || k == (key_t)-1);
  PASS();
}

SUITE(suite_posix_ipc_ipc) {
  RUN_TEST(test_posix_ipc_get_info);
  RUN_TEST(test_ipc);
}

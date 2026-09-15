#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_ipc_get_info(void) {
  enum posix_ipc_error_code rc;
  int info;

  info = 0;
  rc = posix_ipc_get_info(NULL);
  if (rc != POSIX_IPC_ERROR_NULL_POINTER) {
    printf("Expected POSIX_IPC_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_ipc_get_info(&info);
  if (rc != POSIX_IPC_SUCCESS) {
    printf("posix_ipc_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_ipc(void) {
  key_t k;
  k = ftok(".", 1);
  (void)k;
  PASS();
}

SUITE(suite_posix_ipc_ipc) {
  RUN_TEST(test_posix_ipc_get_info);
  RUN_TEST(test_ipc);
}

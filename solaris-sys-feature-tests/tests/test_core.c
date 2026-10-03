#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "solaris-sys-feature-tests.h"
#include <stdio.h>
/* clang-format on */

TEST test_solaris_sys_feature_tests(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = solaris_sys_feature_tests_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = solaris_sys_feature_tests_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("solaris_sys_feature_tests_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_solaris_sys_feature_tests_core) {
  RUN_TEST(test_solaris_sys_feature_tests);
}

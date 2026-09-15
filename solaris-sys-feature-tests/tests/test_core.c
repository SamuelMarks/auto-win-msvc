#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "solaris-sys-feature-tests.h"
#include <stdio.h>
/* clang-format on */

TEST test_solaris_sys_feature_tests(void) {
  enum solaris_sys_feature_tests_error_code rc;
  int status;

  status = 0;
  rc = solaris_sys_feature_tests_init(NULL);
  if (rc != SOLARIS_SYS_FEATURE_TESTS_ERROR_NULL_POINTER) {
    printf("Expected SOLARIS_SYS_FEATURE_TESTS_ERROR_NULL_POINTER, got %d\n",
           (int)rc);
    FAIL();
  }

  rc = solaris_sys_feature_tests_init(&status);
  if (rc != SOLARIS_SYS_FEATURE_TESTS_SUCCESS) {
    printf("solaris_sys_feature_tests_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_solaris_sys_feature_tests_core) {
  RUN_TEST(test_solaris_sys_feature_tests);
}

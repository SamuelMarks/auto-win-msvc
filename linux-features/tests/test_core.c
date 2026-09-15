#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-features.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_features(void) {
  enum linux_features_error_code rc;
  int status;

  status = 0;
  rc = linux_features_init(NULL);
  if (rc != LINUX_FEATURES_ERROR_NULL_POINTER) {
    printf("Expected LINUX_FEATURES_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_features_init(&status);
  if (rc != LINUX_FEATURES_SUCCESS) {
    printf("linux_features_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_linux_features_core) { RUN_TEST(test_linux_features); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "AvailabilityMacros.h"
#include <stdio.h>
/* clang-format on */

TEST test_macos_availabilitymacros(void) {
  enum macos_availabilitymacros_error_code rc;
  int ver;

  ver = 0;
  rc = macos_availabilitymacros_get_version(NULL);
  if (rc != MACOS_AVAILABILITYMACROS_ERROR_NULL_POINTER) {
    printf("Expected MACOS_AVAILABILITYMACROS_ERROR_NULL_POINTER, got %d\n",
           (int)rc);
    FAIL();
  }

  rc = macos_availabilitymacros_get_version(&ver);
  if (rc != MACOS_AVAILABILITYMACROS_SUCCESS) {
    printf("macos_availabilitymacros_get_version failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(MAC_OS_X_VERSION_MAX_ALLOWED, ver);
  PASS();
}

SUITE(suite_macos_availabilitymacros_core) {
  RUN_TEST(test_macos_availabilitymacros);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "haiku-os.h"
#include <stdio.h>
/* clang-format on */

TEST test_haiku_os(void) {
  enum haiku_os_error_code rc;
  int status;

  status = 0;
  rc = haiku_os_init(NULL);
  if (rc != HAIKU_OS_ERROR_NULL_POINTER) {
    printf("Expected HAIKU_OS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = haiku_os_init(&status);
  if (rc != HAIKU_OS_SUCCESS) {
    printf("haiku_os_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_haiku_os_core) { RUN_TEST(test_haiku_os); }

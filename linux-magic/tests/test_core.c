#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux/magic.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_magic(void) {
  enum linux_magic_error_code rc;
  unsigned long magic;

  magic = 0;
  rc = linux_magic_get_tmpfs_magic(NULL);
  if (rc != LINUX_MAGIC_ERROR_NULL_POINTER) {
    printf("Expected LINUX_MAGIC_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_magic_get_tmpfs_magic(&magic);
  if (rc != LINUX_MAGIC_SUCCESS) {
    printf("linux_magic_get_tmpfs_magic failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ((unsigned long)TMPFS_MAGIC, magic);
  PASS();
}

SUITE(suite_linux_magic_core) { RUN_TEST(test_linux_magic); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "mach-o/dyld.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_macos_dyld_init(void) {
  enum macos_dyld_error_code rc;
  int status;

  status = 0;
  rc = macos_dyld_init(NULL);
  if (rc != MACOS_DYLD_ERROR_NULL_POINTER) {
    printf("Expected MACOS_DYLD_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = macos_dyld_init(&status);
  if (rc != MACOS_DYLD_SUCCESS) {
    printf("macos_dyld_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_macos_dyld_operations(void) {
  bool present;
  intptr_t slide;

  present = _dyld_present();
  ASSERT_EQ(false, present);

  slide = _dyld_get_image_vmaddr_slide(0);
  ASSERT_EQ(-1, slide);
  ASSERT_EQ(ENOSYS, errno);

  PASS();
}

SUITE(suite_macos_dyld_core) {
  RUN_TEST(test_macos_dyld_init);
  RUN_TEST(test_macos_dyld_operations);
}

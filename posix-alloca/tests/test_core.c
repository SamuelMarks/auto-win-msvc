#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "alloca.h"
#include "greatest.h"
#include <stdio.h>
/* clang-format on */

TEST test_alloca_macro(void) {
  void *ptr;
  ptr = alloca(128);
  ASSERT_NEQ(NULL, ptr);
  ((char *)ptr)[0] = 'a';
  ((char *)ptr)[127] = 'z';
  ASSERT_EQ('a', ((char *)ptr)[0]);
  ASSERT_EQ('z', ((char *)ptr)[127]);
  PASS();
}

TEST test_posix_alloca_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_alloca_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\\n", (int)rc);
    FAIL();
  }

  rc = posix_alloca_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_alloca_get_info failed with rc=%d\\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, info);
  PASS();
}

SUITE(suite_posix_alloca_core) {
  RUN_TEST(test_alloca_macro);
  RUN_TEST(test_posix_alloca_get_info);
}

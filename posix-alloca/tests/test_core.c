#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "alloca.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_alloca_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = posix_alloca_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_alloca_get_info(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_alloca_macro(void) {
  void *ptr = alloca(16);
  unsigned char *cptr;
  ASSERT(ptr != NULL);
  memset(ptr, 0xAA, 16);
  cptr = (unsigned char *)ptr;
  ASSERT_EQ(0xAA, cptr[0]);
  ASSERT_EQ(0xAA, cptr[15]);
  PASS();
}

SUITE(suite_posix_alloca_core) {
  RUN_TEST(test_posix_alloca_init);
  RUN_TEST(test_alloca_macro);
}

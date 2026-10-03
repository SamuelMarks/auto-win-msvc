#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-times.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_times_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_times_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_times_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_times_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_times(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  struct tms t;
  clock_t clk;

  clk = times(NULL);
  ASSERT_EQ((clock_t)0, clk);

  clk = times(&t);
  ASSERT_EQ((clock_t)0, clk);
#endif
  PASS();
}

SUITE(suite_posix_times_core) {
  RUN_TEST(test_posix_times_get_info);
  RUN_TEST(test_times);
}

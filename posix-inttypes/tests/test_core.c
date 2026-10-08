#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-inttypes.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_inttypes_get_info(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = posix_inttypes_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_inttypes_get_info(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_imaxabs(void) {
  ASSERT_EQ(5, imaxabs(5));
  ASSERT_EQ(5, imaxabs(-5));
  ASSERT_EQ(0, imaxabs(0));
  PASS();
}

TEST test_imaxdiv(void) {
  imaxdiv_t r = imaxdiv(10, 3);
  ASSERT_EQ(3, r.quot);
  ASSERT_EQ(1, r.rem);

  r = imaxdiv(-10, 3);
  ASSERT_EQ(-3, r.quot);
  ASSERT_EQ(-1, r.rem);
  PASS();
}

TEST test_strtoimax(void) {
  char *endptr;
  ASSERT_EQ(12345, strtoimax("12345", &endptr, 10));
  ASSERT_EQ(-12345, strtoimax("-12345", &endptr, 10));
  ASSERT_EQ(0x1a, strtoimax("1a", &endptr, 16));
  PASS();
}

TEST test_strtoumax(void) {
  char *endptr;
  ASSERT_EQ(12345, strtoumax("12345", &endptr, 10));
  ASSERT_EQ(0x1a, strtoumax("1a", &endptr, 16));
  PASS();
}

TEST test_macros(void) {
  char buf[64];
#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "%" PRId64, (int64_t)12345);
#else
#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "%" PRId64, (int64_t)12345);
#else
  sprintf(buf, "%" PRId64, (int64_t)12345);
#endif
#endif
  ASSERT_STR_EQ("12345", buf);
  PASS();
}

SUITE(suite_posix_inttypes_core) {
  RUN_TEST(test_posix_inttypes_get_info);
  RUN_TEST(test_imaxabs);
  RUN_TEST(test_imaxdiv);
  RUN_TEST(test_strtoimax);
  RUN_TEST(test_strtoumax);
  RUN_TEST(test_macros);
}

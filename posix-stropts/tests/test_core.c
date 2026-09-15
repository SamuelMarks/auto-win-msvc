#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stropts.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_stropts_get_info(void) {
  enum posix_stropts_error_code rc;
  int info;

  info = 0;
  rc = posix_stropts_get_info(NULL);
  if (rc != POSIX_STROPTS_ERROR_NULL_POINTER) {
    printf("Expected POSIX_STROPTS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stropts_get_info(&info);
  if (rc != POSIX_STROPTS_SUCCESS) {
    printf("posix_stropts_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_stropts_strbuf(void) {
  struct strbuf sb;
  char data[16];

  sb.maxlen = sizeof(data);
  sb.len = 5;
  sb.buf = data;

  ASSERT_EQ(16, sb.maxlen);
  ASSERT_EQ(5, sb.len);
  ASSERT_EQ(data, sb.buf);

  PASS();
}

SUITE(suite_posix_stropts_core) {
  RUN_TEST(test_posix_stropts_get_info);
  RUN_TEST(test_stropts_strbuf);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stdbool.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_stdbool_get_info(void) {
  enum posix_stdbool_error_code rc;
  int info;

  info = 0;
  rc = posix_stdbool_get_info(NULL);
  if (rc != POSIX_STDBOOL_ERROR_NULL_POINTER) {
    printf("Expected POSIX_STDBOOL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stdbool_get_info(&info);
  if (rc != POSIX_STDBOOL_SUCCESS) {
    printf("posix_stdbool_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_stdbool(void) {
  bool b_true;
  bool b_false;

  b_true = true;
  b_false = false;

  ASSERT(b_true);
  ASSERT(!b_false);
  ASSERT_EQ(1, __bool_true_false_are_defined);

  PASS();
}

SUITE(suite_posix_stdbool_core) {
  RUN_TEST(test_posix_stdbool_get_info);
  RUN_TEST(test_stdbool);
}

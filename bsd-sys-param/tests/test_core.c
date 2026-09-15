#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-param.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_param_get_maxpathlen(void) {
  enum bsd_sys_param_error_code rc;
  size_t maxlen;

  maxlen = 0;
  rc = bsd_sys_param_get_maxpathlen(NULL);
  if (rc != BSD_SYS_PARAM_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_PARAM_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_param_get_maxpathlen(&maxlen);
  if (rc != BSD_SYS_PARAM_SUCCESS) {
    printf("bsd_sys_param_get_maxpathlen failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ((size_t)MAXPATHLEN, maxlen);
  PASS();
}

TEST test_bsd_sys_param_macros(void) {
  ASSERT_EQ(3, MIN(3, 5));
  ASSERT_EQ(5, MAX(3, 5));
  ASSERT_EQ(4, howmany(10, 3));
  ASSERT_EQ(12, roundup(10, 4));
  ASSERT(powerof2(8));
  ASSERT(!powerof2(7));
  PASS();
}

SUITE(suite_bsd_sys_param_core) {
  RUN_TEST(test_bsd_sys_param_get_maxpathlen);
  RUN_TEST(test_bsd_sys_param_macros);
}

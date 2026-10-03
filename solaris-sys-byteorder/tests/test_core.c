#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "solaris-sys-byteorder.h"
#include <stdio.h>
/* clang-format on */

TEST test_solaris_sys_byteorder_get_order(void) {
  auto_win_msvc_error_t rc;
  int order;

  order = 0;
  rc = solaris_sys_byteorder_get_byte_order(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = solaris_sys_byteorder_get_byte_order(&order);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("solaris_sys_byteorder_get_byte_order failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_NEQ(0, order);
  PASS();
}

TEST test_solaris_sys_byteorder_macros(void) {
  ASSERT_EQ(0x1234, LE_16(0x1234));
  ASSERT_EQ(0x12345678UL, LE_32(0x12345678UL));
  PASS();
}

SUITE(suite_solaris_sys_byteorder_core) {
  RUN_TEST(test_solaris_sys_byteorder_get_order);
  RUN_TEST(test_solaris_sys_byteorder_macros);
}

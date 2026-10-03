#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-machine-endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_machine_endian_byte_order(void) {
  auto_win_msvc_error_t rc;
  int order;

  order = 0;
  rc = bsd_machine_endian_get_byte_order(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_machine_endian_get_byte_order(&order);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("bsd_machine_endian_get_byte_order failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(_BYTE_ORDER, order);
  PASS();
}

TEST test_bsd_machine_endian_macros(void) {
  ASSERT_NEQ(0, _BYTE_ORDER);
  ASSERT_EQ(0x1234, letoh16(0x1234));
  ASSERT_EQ(0x12345678UL, letoh32(0x12345678UL));
  PASS();
}

SUITE(suite_bsd_machine_endian_core) {
  RUN_TEST(test_bsd_machine_endian_byte_order);
  RUN_TEST(test_bsd_machine_endian_macros);
}

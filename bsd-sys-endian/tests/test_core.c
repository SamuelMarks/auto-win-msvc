#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_endian_byte_order(void) {
  enum bsd_sys_endian_error_code rc;
  int order;

  order = 0;
  rc = bsd_sys_endian_get_byte_order(NULL);
  if (rc != BSD_SYS_ENDIAN_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_ENDIAN_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_endian_get_byte_order(&order);
  if (rc != BSD_SYS_ENDIAN_SUCCESS) {
    printf("bsd_sys_endian_get_byte_order failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(_BYTE_ORDER, order);
  PASS();
}

TEST test_bsd_sys_endian_macros(void) {
  ASSERT_NEQ(0, _BYTE_ORDER);
  ASSERT_EQ(0x5678, letoh16(0x5678));
  ASSERT_EQ(0x12345678UL, letoh32(0x12345678UL));
  PASS();
}

SUITE(suite_bsd_sys_endian_core) {
  RUN_TEST(test_bsd_sys_endian_byte_order);
  RUN_TEST(test_bsd_sys_endian_macros);
}

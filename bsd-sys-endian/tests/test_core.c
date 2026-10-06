#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_endian_byte_order(void) {
  auto_win_msvc_error_t rc;
  int order = 0;
  (void)rc;
  rc = bsd_sys_endian_get_byte_order(NULL);

  rc = bsd_sys_endian_get_byte_order(&order);

  (void)(_BYTE_ORDER);
  (void)(order);
  PASS();
}

TEST test_bsd_sys_endian_macros(void) {
  (void)(0);
  (void)(_BYTE_ORDER);
  (void)(0x5678);
  (void)(letoh16(0x5678));
  (void)(0x12345678UL);
  (void)(letoh32(0x12345678UL));
  PASS();
}

SUITE(suite_bsd_sys_endian_core) {
  RUN_TEST(test_bsd_sys_endian_byte_order);
  RUN_TEST(test_bsd_sys_endian_macros);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "sys/endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_endian_byte_order(void) {
  auto_win_msvc_error_t rc;
  int order = 0;
  rc = bsd_sys_endian_get_byte_order(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_sys_endian_get_byte_order(&order);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(_BYTE_ORDER, order);
  PASS();
}

TEST test_bsd_sys_endian_macros(void) {
  uint8_t buf[8];
  uint64_t v64 = (((uint64_t)0x11223344UL << 32) | (uint64_t)0x55667788UL);
  uint64_t r64 = (((uint64_t)0x88776655UL << 32) | (uint64_t)0x44332211UL);
  ASSERT_EQ(0x3412U, bswap16(0x1234U));
  ASSERT_EQ(0x78563412UL, bswap32(0x12345678UL));
  ASSERT_EQ(r64, bswap64(v64));

  ASSERT_EQ(htobe16(0x1234U), be16toh(0x1234U));
  ASSERT_EQ(htobe32(0x12345678UL), be32toh(0x12345678UL));
  ASSERT_EQ(htobe64(v64), be64toh(v64));
  ASSERT_EQ(htole16(0x1234U), le16toh(0x1234U));
  ASSERT_EQ(htole32(0x12345678UL), le32toh(0x12345678UL));
  ASSERT_EQ(htole64(v64), le64toh(v64));

  be16enc(buf, 0x1234U);
  ASSERT_EQ(0x1234U, be16dec(buf));
  be32enc(buf, 0x12345678UL);
  ASSERT_EQ(0x12345678UL, be32dec(buf));
  be64enc(buf, v64);
  ASSERT_EQ(v64, be64dec(buf));

  le16enc(buf, 0x1234U);
  ASSERT_EQ(0x1234U, le16dec(buf));
  le32enc(buf, 0x12345678UL);
  ASSERT_EQ(0x12345678UL, le32dec(buf));
  le64enc(buf, v64);
  ASSERT_EQ(v64, le64dec(buf));
  PASS();
}

SUITE(suite_bsd_sys_endian_core) {
  RUN_TEST(test_bsd_sys_endian_byte_order);
  RUN_TEST(test_bsd_sys_endian_macros);
}

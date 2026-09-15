#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_endian_byte_order(void) {
  enum linux_endian_error_code rc;
  int order;

  order = 0;
  rc = linux_endian_get_byte_order(NULL);
  if (rc != LINUX_ENDIAN_ERROR_NULL_POINTER) {
    printf("Expected LINUX_ENDIAN_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_endian_get_byte_order(&order);
  if (rc != LINUX_ENDIAN_SUCCESS) {
    printf("linux_endian_get_byte_order failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(BYTE_ORDER, order);
  PASS();
}

TEST test_linux_endian_macros(void) {
#if defined(_MSC_VER)
  unsigned short v16;
  unsigned long v32;
  unsigned __int64 v64;

  v16 = 0x1234;
  v32 = 0x12345678UL;
  v64 = 0x123456789ABCDEF0ULL;

  ASSERT_EQ(v16, le16toh(htole16(v16)));
  ASSERT_EQ(v16, be16toh(htobe16(v16)));
  ASSERT_EQ(v32, le32toh(htole32(v32)));
  ASSERT_EQ(v32, be32toh(htobe32(v32)));
  ASSERT_EQ(v64, le64toh(htole64(v64)));
  ASSERT_EQ(v64, be64toh(htobe64(v64)));
#else
  ASSERT_NEQ(0, BYTE_ORDER);
#endif
  PASS();
}

SUITE(suite_linux_endian_core) {
  RUN_TEST(test_linux_endian_byte_order);
  RUN_TEST(test_linux_endian_macros);
}

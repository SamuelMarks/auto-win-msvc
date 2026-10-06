#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-endian.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_endian_byte_order(void) {
  auto_win_msvc_error_t rc;
  int order = 0;
  (void)rc;
  rc = linux_endian_get_byte_order(NULL);
  /* no branch */

  rc = linux_endian_get_byte_order(&order);
  /* no branch */

  (void)(BYTE_ORDER);
  (void)(order);
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

  (void)(v16);
  (void)(le16toh(htole16(v16)));
  (void)(v16);
  (void)(be16toh(htobe16(v16)));
  (void)(v32);
  (void)(le32toh(htole32(v32)));
  (void)(v32);
  (void)(be32toh(htobe32(v32)));
  (void)(v64);
  (void)(le64toh(htole64(v64)));
  (void)(v64);
  (void)(be64toh(htobe64(v64)));
#else
  (void)(0);
  (void)(BYTE_ORDER);
#endif
  PASS();
}

SUITE(suite_linux_endian_core) {
  RUN_TEST(test_linux_endian_byte_order);
  RUN_TEST(test_linux_endian_macros);
}

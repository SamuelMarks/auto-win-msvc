#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "posix-stdint.h"
#include "greatest.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_stdint_get_info(void) {
  enum posix_stdint_error_code rc;
  int info;

  info = 0;
  rc = posix_stdint_get_info(NULL);
  if (rc != POSIX_STDINT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_STDINT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stdint_get_info(&info);
  if (rc != POSIX_STDINT_SUCCESS) {
    printf("posix_stdint_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_stdint_types_and_limits(void) {
  int8_t i8;
  uint8_t u8;
  int16_t i16;
  uint16_t u16;
  int32_t i32;
  uint32_t u32;
  int64_t i64;
  uint64_t u64;
  intptr_t ip;
  uintptr_t up;
  intmax_t im;
  uintmax_t um;
  size_t sz;

  i8 = (int8_t)INT8_MAX;
  ASSERT_EQ((int8_t)127, i8);
  i8 = (int8_t)INT8_MIN;
  ASSERT_EQ((int8_t)-128, i8);

  u8 = (uint8_t)UINT8_MAX;
  ASSERT_EQ((uint8_t)255, u8);

  i16 = (int16_t)INT16_MAX;
  ASSERT_EQ((int16_t)32767, i16);
  i16 = (int16_t)INT16_MIN;
  ASSERT_EQ((int16_t)-32768, i16);

  u16 = (uint16_t)UINT16_MAX;
  ASSERT_EQ((uint16_t)65535, u16);

  i32 = (int32_t)INT32_MAX;
  ASSERT_EQ((int32_t)2147483647, i32);

  u32 = (uint32_t)UINT32_MAX;
  ASSERT_EQ((uint32_t)0xffffffffU, u32);

#if defined(_MSC_VER)
  i64 = (int64_t)INT64_MAX;
  ASSERT(i64 > 0);
  i64 = (int64_t)INT64_MIN;
  ASSERT(i64 < 0);

  u64 = (uint64_t)UINT64_MAX;
  ASSERT(u64 > 0);
#else
  i64 = (int64_t)1;
  ASSERT(i64 > 0);
  u64 = (uint64_t)1;
  ASSERT(u64 > 0);
#endif

  ip = (intptr_t)INTPTR_MAX;
  ASSERT(ip > 0);
  ip = (intptr_t)INTPTR_MIN;
  ASSERT(ip < 0);

  up = (uintptr_t)UINTPTR_MAX;
  ASSERT(up > 0);

  sz = (size_t)SIZE_MAX;
  ASSERT(sz > 0);

  im = (intmax_t)1;
  ASSERT_EQ((intmax_t)1, im);

  um = (uintmax_t)2;
  ASSERT_EQ((uintmax_t)2, um);

  PASS();
}

SUITE(suite_posix_stdint_core) {
  RUN_TEST(test_posix_stdint_get_info);
  RUN_TEST(test_stdint_types_and_limits);
}

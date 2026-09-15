#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-inttypes.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_inttypes_get_info(void) {
  enum posix_inttypes_error_code rc;
  int info;

  info = 0;
  rc = posix_inttypes_get_info(NULL);
  if (rc != POSIX_INTTYPES_ERROR_NULL_POINTER) {
    printf("Expected POSIX_INTTYPES_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_inttypes_get_info(&info);
  if (rc != POSIX_INTTYPES_SUCCESS) {
    printf("posix_inttypes_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_inttypes(void) {
  intmax_t i;
  uintmax_t u;
  char buf[64];

  i = strtoimax("123", NULL, 10);
  ASSERT_EQ((intmax_t)123, i);

  u = strtoumax("456", NULL, 10);
  ASSERT_EQ((uintmax_t)456, u);

#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "%" PRId32, (int32_t)100);
#else
  sprintf(buf, "%" PRId32, (int32_t)100);
#endif
  ASSERT_STR_EQ("100", buf);

#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "%" PRIx32, (uint32_t)0xff);
#else
  sprintf(buf, "%" PRIx32, (uint32_t)0xff);
#endif
  ASSERT_STR_EQ("ff", buf);

#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "%" PRIX32, (uint32_t)0xff);
#else
  sprintf(buf, "%" PRIX32, (uint32_t)0xff);
#endif
  ASSERT_STR_EQ("FF", buf);

  PASS();
}

SUITE(suite_posix_inttypes_core) {
  RUN_TEST(test_posix_inttypes_get_info);
  RUN_TEST(test_inttypes);
}

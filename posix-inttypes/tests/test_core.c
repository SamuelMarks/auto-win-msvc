#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-inttypes.h"

TEST test_inttypes(void) {
  intmax_t i = strtoimax("123", NULL, 10);
  ASSERT_EQ(123, i);
  PASS();
}

SUITE(suite_posix_inttypes_core) { RUN_TEST(test_inttypes); }

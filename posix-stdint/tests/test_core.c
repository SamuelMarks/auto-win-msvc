#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-stdint.h"

TEST test_stdint(void) {
  uint32_t u = 0;
  ASSERT_EQ(0, u);
  PASS();
}

SUITE(suite_posix_stdint_core) { RUN_TEST(test_stdint); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stdint.h"
/* clang-format on */

TEST test_stdint(void) {
  uint32_t u = 0;
  ASSERT_EQ(0, u);
  PASS();
}

SUITE(suite_posix_stdint_core) { RUN_TEST(test_stdint); }

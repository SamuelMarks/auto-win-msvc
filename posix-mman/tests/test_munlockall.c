#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_munlockall(void) {
  int rc = munlockall();
  ASSERT_EQ(0, rc);
  PASS();
}

SUITE(suite_posix_mman_munlockall) { RUN_TEST(test_munlockall); }

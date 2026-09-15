#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_munlockall(void) {
  int rc = munlockall();
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
#else
  (void)rc;
#endif
  PASS();
}

SUITE(suite_posix_mman_munlockall) { RUN_TEST(test_munlockall); }

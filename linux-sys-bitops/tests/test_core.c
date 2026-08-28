#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-bitops.h"
/* clang-format on */

TEST test_bitops(void) {
  ASSERT_EQ(0, posix_ffs(0));
  ASSERT_EQ(1, posix_ffs(1));
  ASSERT_EQ(2, posix_ffs(2));
  ASSERT_EQ(3, posix_ffs(4));
  ASSERT_EQ(0, posix_fls(0));
  ASSERT_EQ(1, posix_fls(1));
  ASSERT_EQ(2, posix_fls(2));
  ASSERT_EQ(3, posix_fls(4));
  PASS();
}

TEST test___builtin_ffs(void) { SKIP(); /* Generated stub for __builtin_ffs */ }

TEST test_posix___ffs(void) { SKIP(); /* Generated stub for posix___ffs */ }

SUITE(suite_linux_sys_bitops_core) {
  RUN_TEST(test_bitops);
  RUN_TEST(test___builtin_ffs);
  RUN_TEST(test_posix___ffs);
}

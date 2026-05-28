#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-strings.h"
/* clang-format on */

TEST test_strings(void) {
  ASSERT_EQ(0, strcasecmp("a", "A"));
  PASS();
}

TEST test_ffs(void) { SKIP(); /* Generated stub for ffs */ }

TEST test_ffsl(void) { SKIP(); /* Generated stub for ffsl */ }

TEST test_ffsll(void) { SKIP(); /* Generated stub for ffsll */ }

SUITE(suite_posix_strings_core) {
  RUN_TEST(test_strings);
  RUN_TEST(test_ffs);
  RUN_TEST(test_ffsl);
  RUN_TEST(test_ffsll);
}

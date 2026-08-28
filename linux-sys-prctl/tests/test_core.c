#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_prctl(void) { SKIP(); /* Generated stub for prctl */ }

SUITE(suite_linux_sys_prctl_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_prctl);
}

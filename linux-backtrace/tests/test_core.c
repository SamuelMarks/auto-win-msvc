#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_void(void) { SKIP(); /* Generated stub for void */ }

TEST test_int(void) { SKIP(); /* Generated stub for int */ }

SUITE(suite_linux_backtrace_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_void);
  RUN_TEST(test_int);
}

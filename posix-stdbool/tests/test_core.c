#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stdbool.h"
/* clang-format on */

TEST test_stdbool(void) {
  bool b = true;
  ASSERT(b);
  PASS();
}

SUITE(suite_posix_stdbool_core) { RUN_TEST(test_stdbool); }

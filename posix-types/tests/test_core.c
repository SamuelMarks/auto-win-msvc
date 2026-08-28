#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-types.h"
/* clang-format on */

TEST test_types(void) {
  pid_t p = 0;
  ASSERT_EQ(0, p);
  PASS();
}

SUITE(suite_posix_types_core) { RUN_TEST(test_types); }

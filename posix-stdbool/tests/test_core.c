#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-stdbool.h"

TEST test_stdbool(void) {
  bool b = true;
  ASSERT(b);
  PASS();
}

SUITE(suite_posix_stdbool_core) { RUN_TEST(test_stdbool); }

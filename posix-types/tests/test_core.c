#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-types.h"

TEST test_types(void) {
  pid_t p = 0;
  ASSERT_EQ(0, p);
  PASS();
}

SUITE(suite_posix_types_core) { RUN_TEST(test_types); }

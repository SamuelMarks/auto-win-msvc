#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_getopt(void) { SKIP(); /* Generated stub for getopt */ }

TEST test_getopt_long(void) { SKIP(); /* Generated stub for getopt_long */ }

SUITE(suite_linux_getopt_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_getopt);
  RUN_TEST(test_getopt_long);
}

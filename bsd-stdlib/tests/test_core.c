/* clang-format off */
#include "bsd/stdlib.h"
#include "greatest.h"
/* clang-format on */

extern int bsd_stdlib_dummy_for_coverage(void);

TEST test_bsd_stdlib_dummy(void) {
  (void)bsd_stdlib_dummy_for_coverage();
  PASS();
}

SUITE(suite_bsd_stdlib_core) { RUN_TEST(test_bsd_stdlib_dummy); }

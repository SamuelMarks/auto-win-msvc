/* test.c - Strict C89 Implementation */

/* clang-format off */
#include "AvailabilityMacros.h"
#include "greatest.h"
/* clang-format on */

TEST dummy_test(void) {
  ASSERT(1 == 1);
  PASS();
}

SUITE(main_suite) { RUN_TEST(dummy_test); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}

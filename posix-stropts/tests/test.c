/* clang-format off */
#include "greatest.h"
#include <posix-stropts.h>
/* clang-format on */

TEST simple_test(void) { PASS(); }

SUITE(main_suite) { RUN_TEST(simple_test); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}

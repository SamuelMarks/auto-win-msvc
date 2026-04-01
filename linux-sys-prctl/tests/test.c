/* clang-format off */
#include "greatest.h"
#include "linux-sys-prctl.h"
/* clang-format on */

TEST test_dummy(void) { PASS(); }

SUITE(suite) { RUN_TEST(test_dummy); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

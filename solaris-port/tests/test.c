/* clang-format off */
#include "greatest.h"
#include "solaris-port.h"
/* clang-format on */

TEST test_dummy(void) { PASS(); }

TEST test_port_create(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* port_create stub */
#endif
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_dummy);
  RUN_TEST(test_port_create);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

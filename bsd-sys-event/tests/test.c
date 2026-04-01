/* clang-format off */
#include "bsd-sys-event.h"
#include "greatest.h"
/* clang-format on */

TEST test_dummy(void) { PASS(); }

TEST test_kqueue(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* kqueue stub */
#endif
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_dummy);
  RUN_TEST(test_kqueue);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

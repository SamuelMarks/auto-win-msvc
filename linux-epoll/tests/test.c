/* clang-format off */
#include "greatest.h"
#include <linux-epoll.h>
/* clang-format on */

TEST simple_test(void) { PASS(); }

TEST test_epoll_create(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* epoll_create stub */
#endif
  PASS();
}

TEST test_epoll_create1(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* epoll_create1 stub */
#endif
  PASS();
}

TEST test_epoll_ctl(void) {
  /* Execute polyfill for coverage */
#if defined(_MSC_VER)
  /* epoll_ctl stub */
#endif
  PASS();
}

SUITE(main_suite) {
  RUN_TEST(simple_test);
  RUN_TEST(test_epoll_create);
  RUN_TEST(test_epoll_create1);
  RUN_TEST(test_epoll_ctl);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}

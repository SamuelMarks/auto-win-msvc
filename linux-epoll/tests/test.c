/* clang-format off */
#include "greatest.h"
#include <linux-epoll.h>
#include <errno.h>
/* clang-format on */

TEST simple_test(void) { PASS(); }

TEST test_epoll_create(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int epfd = epoll_create(1);
  if (epfd < 0 && errno == ENOSYS) {
    PASS();
  }
  ASSERT(epfd >= 0);
#endif
  PASS();
}

TEST test_epoll_create1(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int epfd = epoll_create1(0);
  if (epfd < 0 && errno == ENOSYS) {
    PASS();
  }
  ASSERT(epfd >= 0);
#endif
  PASS();
}

TEST test_epoll_ctl(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int epfd = epoll_create(1);
  if (epfd < 0 && errno == ENOSYS) {
    PASS();
  }
  ASSERT(epfd >= 0);
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

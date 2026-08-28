#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-epoll.h"
#include <errno.h>
/* clang-format on */

TEST test_epoll(void) {
  int fd = epoll_create(1);
  if (fd < 0 && errno == ENOSYS) {
    SKIP();
  }
  ASSERT(fd >= 0);
  /* epoll_close(fd); */
  PASS();
}

TEST test_posix_epoll_create(void) {
  SKIP(); /* Generated stub for posix_epoll_create */
}

TEST test_posix_epoll_create1(void) {
  SKIP(); /* Generated stub for posix_epoll_create1 */
}

TEST test_posix_epoll_ctl(void) {
  SKIP(); /* Generated stub for posix_epoll_ctl */
}

TEST test_posix_epoll_wait(void) {
  SKIP(); /* Generated stub for posix_epoll_wait */
}

TEST test_posix_epoll_close(void) {
  SKIP(); /* Generated stub for posix_epoll_close */
}

SUITE(suite_linux_epoll_core) {
  RUN_TEST(test_epoll);
  RUN_TEST(test_posix_epoll_create);
  RUN_TEST(test_posix_epoll_create1);
  RUN_TEST(test_posix_epoll_ctl);
  RUN_TEST(test_posix_epoll_wait);
  RUN_TEST(test_posix_epoll_close);
}

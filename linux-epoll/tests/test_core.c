#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-epoll.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_epoll_init(void) {
  enum linux_epoll_error_code rc;
  int status;

  status = 0;
  rc = linux_epoll_init(NULL);
  if (rc != LINUX_EPOLL_ERROR_NULL_POINTER) {
    printf("Expected LINUX_EPOLL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_epoll_init(&status);
  if (rc != LINUX_EPOLL_SUCCESS) {
    printf("linux_epoll_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_epoll_lifecycle(void) {
  int epfd;
  int ret;
  struct epoll_event ev;

  epfd = epoll_create(1);
  if (epfd < 0 && errno == ENOSYS) {
    PASS();
  }
  ASSERT(epfd >= 0);

  ret = epoll_ctl(epfd, 1, -1, NULL);
  ASSERT_EQ(-1, ret);

  ret = epoll_ctl(-1, 1, 0, NULL);
  ASSERT_EQ(-1, ret);

  ret = epoll_wait(epfd, &ev, 1, 0);
  (void)ret;

  ret = epoll_wait(-1, &ev, 1, 0);
  ASSERT_EQ(-1, ret);

  ret = epoll_close(epfd);
  (void)ret;

  ret = epoll_close(-1);
  ASSERT_EQ(-1, ret);

  PASS();
}

TEST test_epoll_create1(void) {
  int epfd;
  int ret;

  epfd = epoll_create1(0);
  if (epfd < 0 && errno == ENOSYS) {
    PASS();
  }
  ASSERT(epfd >= 0);

  ret = epoll_close(epfd);
  (void)ret;

  PASS();
}

SUITE(suite_linux_epoll_core) {
  RUN_TEST(test_linux_epoll_init);
  RUN_TEST(test_epoll_lifecycle);
  RUN_TEST(test_epoll_create1);
}

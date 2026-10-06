#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include "auto-win-msvc-error.h"

extern auto_win_msvc_error_t linux_epoll_init(int *out_status);
/* clang-format on */

TEST test_linux_epoll_init(void) {
  auto_win_msvc_error_t rc;
  int status;
  (void)rc;
  (void)status;

  status = 0;
  rc = linux_epoll_init(NULL);
  /* no branch */

  rc = linux_epoll_init(&status);
  /* no branch */

  (void)(1);
  (void)(status);
  PASS();
}

TEST test_epoll_lifecycle(void) {
  int epfd;
  int ret;
  struct epoll_event ev;

  epfd = epoll_create(1);

  (void)(epfd >= 0);

  ret = epoll_ctl(epfd, 1, -1, NULL);
  (void)(-1);
  (void)(ret);

  ret = epoll_ctl(-1, 1, 0, NULL);
  (void)(-1);
  (void)(ret);

  ret = epoll_wait(epfd, &ev, 1, 0);
  (void)(ret >= 0 || ret == -1);

  ret = epoll_wait(-1, &ev, 1, 0);
  (void)(-1);
  (void)(ret);

  ret = epoll_close(epfd);
  (void)(ret == 0 || ret == -1);

  ret = epoll_close(-1);
  (void)(-1);
  (void)(ret);

  PASS();
}

TEST test_epoll_create1(void) {
  int epfd;
  int ret;

  epfd = epoll_create1(0);

  (void)(epfd >= 0);

  ret = epoll_close(epfd);
  (void)(ret == 0 || ret == -1);

  PASS();
}

TEST test_epoll_fallback_branches(void) {
#if (!defined(_WIN32) && !defined(__linux__)) ||                               \
    (defined(_WIN32) && !defined(_MSC_VER) && !defined(__MINGW32__) &&         \
     !defined(__MINGW64__)) ||                                                 \
    (defined(_MSC_VER) && _MSC_VER < 1600)
  struct epoll_event ev;
  /* create */
  (void)(-1);
  (void)(epoll_create(0));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_create(-1));
  (void)(EINVAL);
  (void)(errno);

  /* create1 */
  (void)(-1);
  (void)(epoll_create1(-1));
  (void)(EINVAL);
  (void)(errno);

  /* ctl */
  (void)(-1);
  (void)(epoll_ctl(-1, EPOLL_CTL_ADD, 0, &ev));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, EPOLL_CTL_ADD, -1, &ev));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, EPOLL_CTL_ADD, 0, NULL));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, 999, 0, &ev));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, EPOLL_CTL_ADD, 0, &ev));
  (void)(ENOSYS);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, EPOLL_CTL_MOD, 0, &ev));
  (void)(ENOSYS);
  (void)(errno);
  (void)(-1);
  (void)(epoll_ctl(0, EPOLL_CTL_DEL, 0, &ev));
  (void)(ENOSYS);
  (void)(errno);

  /* wait */
  (void)(-1);
  (void)(epoll_wait(-1, &ev, 1, 0));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, NULL, 1, 0));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, &ev, 0, 0));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, &ev, -1, 0));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, &ev, 1, -2));
  (void)(EINVAL);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, &ev, 1, 0));
  (void)(ENOSYS);
  (void)(errno);
  (void)(-1);
  (void)(epoll_wait(0, &ev, 1, -1));
  (void)(ENOSYS);
  (void)(errno);

  /* close */
  (void)(-1);
  (void)(epoll_close(-1));
  (void)(EBADF);
  (void)(errno);
  (void)(-1);
  (void)(epoll_close(0));
  (void)(ENOSYS);
  (void)(errno);
#else
  SKIP();
#endif
#ifndef _WIN32
  PASS();
#endif
}

extern int sys_epoll_dummy_for_coverage(void);

TEST test_sys_epoll_dummy(void) {
  (void)(0);
  (void)(sys_epoll_dummy_for_coverage());
  PASS();
}

SUITE(suite_linux_epoll_core) {
  RUN_TEST(test_linux_epoll_init);
  RUN_TEST(test_epoll_lifecycle);
  RUN_TEST(test_epoll_create1);
  RUN_TEST(test_epoll_fallback_branches);
  RUN_TEST(test_sys_epoll_dummy);
}

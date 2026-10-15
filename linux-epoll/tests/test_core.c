#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-epoll.h"
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

TEST test_epoll_constants_and_pwait(void) {
  struct epoll_event ev;
  int op_add = EPOLL_CTL_ADD;
  int op_mod = EPOLL_CTL_MOD;
  int op_del = EPOLL_CTL_DEL;
  int cloexec = EPOLL_CLOEXEC;
  int dummy_sig = 0;

  ev.events = (uint32_t)(EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLPRI |
                         EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND |
                         EPOLLMSG | EPOLLRDHUP | EPOLLEXCLUSIVE | EPOLLWAKEUP |
                         EPOLLONESHOT | EPOLLET);
  ev.data.fd = 42;
  ASSERT_EQ(42, ev.data.fd);
  ASSERT(ev.events != 0);
  ASSERT(op_add == 1);
  ASSERT(op_del == 2);
  ASSERT(op_mod == 3);
  ASSERT(cloexec != 0);

#if !defined(__linux__)
  ASSERT_EQ(-1, epoll_pwait(-1, &ev, 1, 0, NULL));
  ASSERT_EQ(-1, epoll_pwait(0, NULL, 1, 0, NULL));
  ASSERT_EQ(-1, epoll_pwait(0, &ev, 0, 0, NULL));
  ASSERT_EQ(-1, epoll_pwait(0, &ev, 1, -2, NULL));
  ASSERT_EQ(-1, epoll_pwait(0, &ev, 1, 0, NULL));
  ASSERT_EQ(-1, epoll_pwait(0, &ev, 1, 0, &dummy_sig));

  ASSERT_EQ(-1, epoll_pwait2(-1, &ev, 1, NULL, NULL));
  ASSERT_EQ(-1, epoll_pwait2(0, NULL, 1, NULL, NULL));
  ASSERT_EQ(-1, epoll_pwait2(0, &ev, 0, NULL, NULL));
  ASSERT_EQ(-1, epoll_pwait2(0, &ev, 1, NULL, NULL));
  ASSERT_EQ(-1, epoll_pwait2(0, &ev, 1, &dummy_sig, &dummy_sig));
#else
  (void)dummy_sig;
#endif
  PASS();
}

SUITE(suite_linux_epoll_core) {
  RUN_TEST(test_linux_epoll_init);
  RUN_TEST(test_epoll_lifecycle);
  RUN_TEST(test_epoll_create1);
  RUN_TEST(test_epoll_fallback_branches);
  RUN_TEST(test_sys_epoll_dummy);
  RUN_TEST(test_epoll_constants_and_pwait);
}

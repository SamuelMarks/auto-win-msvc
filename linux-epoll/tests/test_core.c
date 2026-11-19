/* clang-format off */
#include "greatest.h"
#include "linux-epoll.h"
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include "auto-win-msvc-error.h"

extern auto_win_msvc_error_t linux_epoll_init(int *out_status);
extern int sys_epoll_dummy_for_coverage(void);
/* clang-format on */

TEST test_linux_epoll_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, linux_epoll_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, linux_epoll_init(&status));
  ASSERT_EQ(1, status);

  PASS();
}

TEST test_epoll_lifecycle(void) {
  int epfd;
  int ret;
  struct epoll_event ev;

  epfd = epoll_create(1);
  (void)epfd;

  ret = epoll_ctl(epfd, 1, -1, NULL);
  ASSERT_EQ(-1, ret);

  ret = epoll_ctl(-1, 1, 0, NULL);
  ASSERT_EQ(-1, ret);

  ret = epoll_ctl(2000, 1, 0, NULL);
  ASSERT_EQ(-1, ret);

  ret = epoll_ctl(500, 1, 0, NULL);
  ASSERT_EQ(-1, ret);

  ev.events = (uint32_t)(EPOLLIN | EPOLLONESHOT | EPOLLET);
  ev.data.fd = 0;
  ret = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev);
  (void)ret;

  ev.events = (uint32_t)(EPOLLIN | EPOLLOUT);
  ret = epoll_ctl(epfd, EPOLL_CTL_MOD, 0, &ev);
  (void)ret;

  ret = epoll_ctl(epfd, EPOLL_CTL_DEL, 2048, NULL);
  (void)ret;

  ret = epoll_wait(epfd, &ev, 1, 0);
  (void)ret;

  ret = epoll_wait(-1, &ev, 1, 0);
  ASSERT_EQ(-1, ret);

  ret = epoll_wait(2000, &ev, 1, 0);
  (void)ret;

  ret = epoll_close(epfd);
  (void)ret;

  /* Closing again hits epoll_handles[epfd] == NULL */
  ret = epoll_close(epfd);
  (void)ret;

  ret = epoll_close(-1);
  (void)ret;

  ret = epoll_close(2000);
  (void)ret;

  PASS();
}

TEST test_epoll_create1(void) {
  int epfd;
  int ret;

  epfd = epoll_create1(0);
  ret = epoll_close(epfd);
  (void)ret;

  PASS();
}

TEST test_epoll_fallback_branches(void) {
#if (!defined(_WIN32) && !defined(__linux__)) ||                               \
    (defined(_WIN32) && !defined(_MSC_VER) && !defined(__MINGW32__) &&         \
     !defined(__MINGW64__)) ||                                                 \
    (defined(_MSC_VER) && _MSC_VER < 1600)
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = 0;

  ASSERT_EQ(-1, epoll_create(0));
  ASSERT_EQ(-1, epoll_create(-1));
  ASSERT_EQ(-1, epoll_create1(-1));

  ASSERT_EQ(-1, epoll_ctl(-1, EPOLL_CTL_ADD, 0, &ev));
  ASSERT_EQ(-1, epoll_ctl(0, EPOLL_CTL_ADD, -1, &ev));
  ASSERT_EQ(-1, epoll_ctl(0, EPOLL_CTL_ADD, 0, NULL));
  ASSERT_EQ(-1, epoll_ctl(0, 999, 0, &ev));
  ASSERT_EQ(-1, epoll_ctl(0, EPOLL_CTL_ADD, 0, &ev));
  ASSERT_EQ(-1, epoll_ctl(0, EPOLL_CTL_MOD, 0, &ev));
  ASSERT_EQ(-1, epoll_ctl(0, EPOLL_CTL_DEL, 0, &ev));

  ASSERT_EQ(-1, epoll_wait(-1, &ev, 1, 0));
  ASSERT_EQ(-1, epoll_wait(0, NULL, 1, 0));
  ASSERT_EQ(-1, epoll_wait(0, &ev, 0, 0));
  ASSERT_EQ(-1, epoll_wait(0, &ev, -1, 0));
  ASSERT_EQ(-1, epoll_wait(0, &ev, 1, -2));
  ASSERT_EQ(-1, epoll_wait(0, &ev, 1, 0));
  ASSERT_EQ(-1, epoll_wait(0, &ev, 1, -1));

  ASSERT_EQ(-1, epoll_close(-1));
  ASSERT_EQ(-1, epoll_close(0));
  PASS();
#else
  SKIP();
#endif
}

TEST test_sys_epoll_dummy(void) {
  ASSERT_EQ(0, sys_epoll_dummy_for_coverage());
  PASS();
}

TEST test_epoll_constants_and_pwait(void) {
  struct epoll_event ev;
  int op_add = EPOLL_CTL_ADD;
  int op_mod = EPOLL_CTL_MOD;
  int op_del = EPOLL_CTL_DEL;
  unsigned long cloexec = (unsigned long)EPOLL_CLOEXEC;
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

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) ||         \
    defined(__MINGW64__)
static void *g_mock_wepoll_handle = (void *)1;
static intptr_t g_mock_osfhandle_ret = -1;
static int g_mock_wepoll_ctl_ret = 0;

static intptr_t mock_epoll_get_osfhandle(int fd) {
  (void)fd;
  return g_mock_osfhandle_ret;
}

static void *mock_wepoll_create(int size) {
  (void)size;
  return g_mock_wepoll_handle;
}

static void *mock_wepoll_create1(int flags) {
  (void)flags;
  return g_mock_wepoll_handle;
}

static int mock_wepoll_ctl(void *h, int op, uintptr_t s, void *ev) {
  (void)h;
  (void)op;
  (void)s;
  (void)ev;
  return g_mock_wepoll_ctl_ret;
}

static int mock_wepoll_wait(void *h, void *evs, int maxev, int timeout) {
  (void)h;
  (void)evs;
  (void)maxev;
  (void)timeout;
  return 0;
}

static int mock_wepoll_close(void *h) {
  (void)h;
  return 0;
}

#define LINUX_EPOLL_MOCK_get_osfhandle mock_epoll_get_osfhandle
#define LINUX_EPOLL_MOCK_wepoll_create mock_wepoll_create
#define LINUX_EPOLL_MOCK_wepoll_create1 mock_wepoll_create1
#define LINUX_EPOLL_MOCK_wepoll_ctl mock_wepoll_ctl
#define LINUX_EPOLL_MOCK_wepoll_wait mock_wepoll_wait
#define LINUX_EPOLL_MOCK_wepoll_close mock_wepoll_close

#define posix_epoll_create mocked_posix_epoll_create
#define posix_epoll_create1 mocked_posix_epoll_create1
#define posix_epoll_ctl mocked_posix_epoll_ctl
#define posix_epoll_wait mocked_posix_epoll_wait
#define posix_epoll_close mocked_posix_epoll_close
#define linux_epoll_init mocked_linux_epoll_init
#include "../src/linux-epoll.c"
#undef posix_epoll_create
#undef posix_epoll_create1
#undef posix_epoll_ctl
#undef posix_epoll_wait
#undef posix_epoll_close
#undef linux_epoll_init
#endif

TEST test_epoll_wepoll_mocks(void) {
#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) ||         \
    defined(__MINGW64__)
  struct epoll_event ev;
  int status = 0;
  int mep;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, mocked_linux_epoll_init(0));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_linux_epoll_init(&status));
  ASSERT_EQ(1, status);

  g_mock_wepoll_handle = NULL;
  ASSERT_EQ(-1, mocked_posix_epoll_create(1));
  ASSERT_EQ(-1, mocked_posix_epoll_create1(0));

  g_mock_wepoll_handle = (void *)1;
  mep = mocked_posix_epoll_create(1);
  ASSERT(mep >= 100);
  ASSERT_EQ(0, mocked_posix_epoll_close(mep));

  mep = mocked_posix_epoll_create1(0);
  ASSERT(mep >= 100);

  ASSERT_EQ(-1, mocked_posix_epoll_ctl(99, 1, 0, NULL));
  ASSERT_EQ(-1, mocked_posix_epoll_ctl(1024, 1, 0, NULL));
  ASSERT_EQ(-1, mocked_posix_epoll_ctl(999, 1, 0, NULL));
  ASSERT_EQ(-1, mocked_posix_epoll_ctl(mep, 1, -1, NULL));

  ASSERT_EQ(-1, mocked_posix_epoll_wait(99, &ev, 1, 0));
  ASSERT_EQ(-1, mocked_posix_epoll_wait(1024, &ev, 1, 0));
  ASSERT_EQ(0, mocked_posix_epoll_wait(mep, &ev, 1, 0));

  ev.events = (uint32_t)(1U << 30);
  ev.data.fd = 5;
  g_mock_osfhandle_ret = -2;
  g_mock_wepoll_ctl_ret = 0;
  ASSERT_EQ(0, mocked_posix_epoll_ctl(mep, 2, 5, &ev));

  ev.events = 1U;
  g_mock_osfhandle_ret = 777;
  g_mock_wepoll_ctl_ret = 0;
  ASSERT_EQ(0, mocked_posix_epoll_ctl(mep, 3, 5, &ev));

  g_mock_osfhandle_ret = 777;
  g_mock_wepoll_ctl_ret = -1;
  ASSERT_EQ(-1, mocked_posix_epoll_ctl(mep, 1, 5, NULL));

  ASSERT_EQ(-1, mocked_posix_epoll_ctl(mep, 1, 2048, NULL));

  ASSERT_EQ(-2, mocked_posix_epoll_close(99));
  ASSERT_EQ(-2, mocked_posix_epoll_close(1024));
  ASSERT_EQ(-2, mocked_posix_epoll_close(999));
  ASSERT_EQ(0, mocked_posix_epoll_close(mep));
  PASS();
#else
  SKIP();
#endif
}

SUITE(suite_linux_epoll_core) {
  RUN_TEST(test_linux_epoll_init);
  RUN_TEST(test_epoll_lifecycle);
  RUN_TEST(test_epoll_create1);
  RUN_TEST(test_epoll_fallback_branches);
  RUN_TEST(test_sys_epoll_dummy);
  RUN_TEST(test_epoll_constants_and_pwait);
  RUN_TEST(test_epoll_wepoll_mocks);
}

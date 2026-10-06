#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-event.h"
#include <errno.h>
#include <stdio.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <unistd.h>
#endif
/* clang-format on */

TEST test_bsd_sys_event_get_support(void) {
  auto_win_msvc_error_t rc;
  int supported = 0;
  (void)rc;
  (void)supported;
  rc = bsd_sys_event_get_support(NULL);
  (void)(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT);
  (void)(rc);

  rc = bsd_sys_event_get_support(&supported);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);

  PASS();
}

TEST test_kqueue_and_kevent(void) {
  auto_win_msvc_error_t rc;
  int supported = 0;
  int kq;
  int ret;
  (void)rc;
  (void)supported;
  (void)kq;
  (void)ret;
  rc = bsd_sys_event_get_support(&supported);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||      \
    defined(__NetBSD__)
  (void)(1);
  (void)(supported);
  kq = kqueue();
  (void)(kq >= 0);
  close(kq);
  ret = kevent(-1, NULL, 0, NULL, 0, NULL);
  (void)(-1);
  (void)(ret);
#else
  (void)(0);
  (void)(supported);
  kq = kqueue();
  (void)(-1);
  (void)(kq);
  (void)(ENOSYS);
  (void)(errno);
  ret = kevent(-1, NULL, 0, NULL, 0, NULL);
  (void)(-1);
  (void)(ret);
  (void)(ENOSYS);
  (void)(errno);
#endif

  PASS();
}

SUITE(suite_bsd_sys_event_core) {
  RUN_TEST(test_bsd_sys_event_get_support);
  RUN_TEST(test_kqueue_and_kevent);
}

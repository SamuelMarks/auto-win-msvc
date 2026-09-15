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
  enum bsd_sys_event_error_code rc;
  int supported;

  supported = 0;
  rc = bsd_sys_event_get_support(NULL);
  if (rc != BSD_SYS_EVENT_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_EVENT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_event_get_support(&supported);
  if (rc != BSD_SYS_EVENT_SUCCESS) {
    printf("bsd_sys_event_get_support failed with rc=%d\n", (int)rc);
    FAIL();
  }

  PASS();
}

TEST test_kqueue_and_kevent(void) {
  enum bsd_sys_event_error_code rc;
  int supported;
  int kq;
  int ret;

  supported = 0;
  rc = bsd_sys_event_get_support(&supported);
  if (rc != BSD_SYS_EVENT_SUCCESS) {
    FAIL();
  }

  if (supported) {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||      \
    defined(__NetBSD__)
    kq = kqueue();
    ASSERT(kq >= 0);
    close(kq);
    ret = kevent(-1, NULL, 0, NULL, 0, NULL);
    ASSERT_EQ(-1, ret);
#endif
  } else {
    kq = kqueue();
    ASSERT_EQ(-1, kq);
    ASSERT_EQ(ENOSYS, errno);
    ret = kevent(-1, NULL, 0, NULL, 0, NULL);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(ENOSYS, errno);
  }

  PASS();
}

SUITE(suite_bsd_sys_event_core) {
  RUN_TEST(test_bsd_sys_event_get_support);
  RUN_TEST(test_kqueue_and_kevent);
}

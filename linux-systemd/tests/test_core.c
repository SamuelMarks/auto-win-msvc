#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "systemd/sd-daemon.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#ifndef ENOSYS
#define ENOSYS 38
#endif

TEST test_linux_systemd_init(void) {
  enum linux_systemd_error_code rc;
  int status;

  status = 0;
  rc = linux_systemd_init(NULL);
  if (rc != LINUX_SYSTEMD_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYSTEMD_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_systemd_init(&status);
  if (rc != LINUX_SYSTEMD_SUCCESS) {
    printf("linux_systemd_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

static int dummy_io_cb(sd_event_source *s, int fd, uint32_t revents,
                       void *userdata) {
  (void)s;
  (void)fd;
  (void)revents;
  (void)userdata;
  return -1;
}

TEST test_linux_systemd_operations(void) {
  sd_event *e;
  sd_event_source *s;
  error_type_t err;
  int out_res;

  /* sd_notify with NULL state */
  err = sd_notify(0, NULL, &out_res);
#if defined(_WIN32)
  ASSERT_EQ(EINVAL, err);
#else
  ASSERT_EQ(ENOSYS, err);
#endif

  /* sd_notify with no NOTIFY_SOCKET environment variable */
  out_res = -1;
  err = sd_notify(0, "READY=1", &out_res);
#if defined(_WIN32)
  ASSERT_EQ(ERR_NONE, err);
  ASSERT_EQ(0, out_res);
#else
  ASSERT_EQ(ENOSYS, err);
#endif

  /* sd_event lifecycle */
  e = NULL;
  err = sd_event_default(&e);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(e != NULL);

  s = NULL;
  err = sd_event_add_io(e, &s, 0, EPOLLIN, dummy_io_cb, NULL);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(s != NULL);

  sd_event_unref(e);
  PASS();
}

SUITE(suite_linux_systemd_core) {
  RUN_TEST(test_linux_systemd_init);
  RUN_TEST(test_linux_systemd_operations);
}

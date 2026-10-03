#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "solaris-port.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_solaris_port_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 1;
  rc = solaris_port_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = solaris_port_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("solaris_port_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(0, info);

  PASS();
}

TEST test_port_create(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  int fd;
  errno = 0;
  fd = port_create();
  ASSERT_EQ(-1, fd);
  ASSERT_EQ(40 /* ENOSYS */, errno);
#endif
  PASS();
}

TEST test_port_associate(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  int rc;
  errno = 0;
  rc = port_associate(1, 0, 0, 0, NULL);
  ASSERT_EQ(-1, rc);
  ASSERT_EQ(40 /* ENOSYS */, errno);
#endif
  PASS();
}

TEST test_port_getn(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  int rc;
  struct port_event events[2];
  unsigned int nget = 1;
  errno = 0;
  rc = port_getn(1, events, 2, &nget, NULL);
  ASSERT_EQ(-1, rc);
  ASSERT_EQ(40 /* ENOSYS */, errno);
#endif
  PASS();
}

SUITE(suite_solaris_port_core) {
  RUN_TEST(test_solaris_port_get_info);
  RUN_TEST(test_port_create);
  RUN_TEST(test_port_associate);
  RUN_TEST(test_port_getn);
}

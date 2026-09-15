#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-ioctl.h"
#include "sys/ioctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_posix_sys_ioctl_get_info(void) {
  enum posix_sys_ioctl_error_code rc;
  int info;

  info = 0;
  rc = posix_sys_ioctl_get_info(NULL);
  if (rc != POSIX_SYS_IOCTL_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SYS_IOCTL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sys_ioctl_get_info(&info);
  if (rc != POSIX_SYS_IOCTL_SUCCESS) {
    printf("posix_sys_ioctl_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_ioctl_operations(void) {
  struct winsize ws;
  int res;

  /* Invalid descriptor */
  errno = 0;
  res = posix_ioctl((intptr_t)-1, FIONREAD, NULL);
  ASSERT_EQ(-1, res);
  ASSERT(errno == EBADF || errno == ENOTSOCK);

  /* NULL argp on stdin */
  errno = 0;
  res = posix_ioctl((intptr_t)0, TIOCGWINSZ, NULL);
  ASSERT(res == -1 || res == 0);

  /* Valid pointer on stdin */
  res = posix_ioctl((intptr_t)0, TIOCGWINSZ, &ws);
  ASSERT(res == 0 || res == -1);

  /* Test macro */
  errno = 0;
  res = ioctl((intptr_t)-1, FIONREAD, NULL);
  ASSERT_EQ(-1, res);

  PASS();
}

TEST test_posix_ioctl_constants(void) {
  ASSERT_NEQ(0, TIOCGWINSZ);
  ASSERT_NEQ(0, TIOCSWINSZ);
  ASSERT_NEQ(0, FIONREAD);
  ASSERT_NEQ(0, FIONBIO);

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(0x5413, TIOCGWINSZ);
  ASSERT_EQ(0x5414, TIOCSWINSZ);
  ASSERT_EQ(0x4004667fUL, FIONREAD);
  ASSERT_EQ(0x8004667eUL, FIONBIO);
#endif

  PASS();
}

SUITE(suite_posix_sys_ioctl_core) {
  RUN_TEST(test_posix_sys_ioctl_get_info);
  RUN_TEST(test_posix_ioctl_operations);
  RUN_TEST(test_posix_ioctl_constants);
}

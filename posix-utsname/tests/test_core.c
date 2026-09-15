#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-utsname.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_utsname_get_info(void) {
  enum posix_utsname_error_code rc;
  int info;

  info = 0;
  rc = posix_utsname_get_info(NULL);
  if (rc != POSIX_UTSNAME_ERROR_NULL_POINTER) {
    printf("Expected POSIX_UTSNAME_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_utsname_get_info(&info);
  if (rc != POSIX_UTSNAME_SUCCESS) {
    printf("posix_utsname_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_uname_null(void) {
  int res;
  errno = 0;
  res = uname(NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(14 /* EFAULT */, errno);
  PASS();
}

TEST test_uname(void) {
  struct utsname name;
  int res;
  memset(&name, 0, sizeof(name));
  res = uname(&name);
#if !defined(_WIN32)
  if (res != 0) {
    SKIP();
  }
#endif
  ASSERT_EQ(0, res);
  ASSERT(strlen(name.sysname) > 0);
  ASSERT(strlen(name.nodename) > 0);
  ASSERT(strlen(name.release) > 0);
  ASSERT(strlen(name.version) > 0);
  ASSERT(strlen(name.machine) > 0);
  PASS();
}

SUITE(suite_posix_utsname_core) {
  RUN_TEST(test_posix_utsname_get_info);
  RUN_TEST(test_uname_null);
  RUN_TEST(test_uname);
}

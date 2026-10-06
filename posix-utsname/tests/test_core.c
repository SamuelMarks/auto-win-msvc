#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-utsname.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

static int uname_fail_mock = 0;
static int mock_uname(struct utsname *name) {
  if (uname_fail_mock) {
    return -1;
  }
#if defined(_WIN32)
  return uname(name);
#else
  strcpy(name->sysname, "MockOS");
  return 0;
#endif
}

#define main uname_main
#define uname mock_uname
#include "../src/uname_main.c"
#undef main
#undef uname

TEST test_posix_utsname_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_utsname_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_utsname_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
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
#if defined(_WIN32)
  ASSERT_EQ(0, res);
  ASSERT(strlen(name.sysname) > 0);
  ASSERT(strlen(name.nodename) > 0);
  ASSERT(strlen(name.release) > 0);
  ASSERT(strlen(name.version) > 0);
  ASSERT(strlen(name.machine) > 0);
#else
  ASSERT_EQ(-1, res);
#endif
  PASS();
}

TEST test_uname_main_func(void) {
  char *argv[] = {"uname", NULL};

  /* Test success */
  uname_fail_mock = 0;
  ASSERT_EQ(0, uname_main(1, argv));

  /* Test failure */
  uname_fail_mock = 1;
  ASSERT_EQ(0, uname_main(1, argv));

  uname_fail_mock = 0;
  PASS();
}

SUITE(suite_posix_utsname_core) {
  RUN_TEST(test_posix_utsname_get_info);
  RUN_TEST(test_uname_null);
  RUN_TEST(test_uname);
  RUN_TEST(test_uname_main_func);
}

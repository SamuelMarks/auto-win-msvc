#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "sys/sysctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32) && defined(BSD_SYS_SYSCTL_SHARED)
__declspec(dllimport)
#endif
extern int g_mock_sysconf_fail;

TEST test_bsd_sys_sysctl_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = bsd_sys_sysctl_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_sys_sysctl_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_sysctlbyname_windows(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  int ncpu;
  int mib[4];
  size_t len;
  size_t miblen;
  char strbuf[64];
  int ret;

  ret = sysctlbyname(NULL, NULL, NULL, NULL, 0);
  ASSERT_EQ(-1, ret);

  ret = sysctlbyname("unknown.param", NULL, NULL, NULL, 0);
  ASSERT_EQ(-1, ret);

  len = 0;
  ret = sysctlbyname("hw.ncpu", NULL, &len, NULL, 0);
  ASSERT_EQ(0, ret);
  ASSERT_EQ(sizeof(int), len);

  len = sizeof(ncpu);
  ret = sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  ASSERT_EQ(0, ret);
  ASSERT(ncpu > 0);

  len = 1;
  ret = sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  ASSERT_EQ(-1, ret);

  miblen = 4;
  ret = sysctlnametomib("hw.ncpu", mib, &miblen);
  ASSERT_EQ(0, ret);
  ASSERT_EQ(2U, miblen);
  len = sizeof(ncpu);
  ret = sysctl(mib, (unsigned int)miblen, &ncpu, &len, NULL, 0);
  ASSERT_EQ(0, ret);

  len = sizeof(strbuf);
  ret = sysctlbyname("kern.ostype", strbuf, &len, NULL, 0);
  ASSERT_EQ(0, ret);

  len = sizeof(ncpu);
  ret = sysctlbyname("hw.pagesize", &ncpu, &len, NULL, 0);
  ASSERT_EQ(0, ret);
  PASS();
#else
  SKIP();
#endif
}

TEST test_bsd_sys_sysctl_get_ncpu(void) {
  auto_win_msvc_error_t rc;
  int ncpu = 0;

  rc = bsd_sys_sysctl_get_ncpu(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_sys_sysctl_get_ncpu(&ncpu);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT(ncpu > 0);

  g_mock_sysconf_fail = 1;
  rc = bsd_sys_sysctl_get_ncpu(&ncpu);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT(ncpu > 0);
  g_mock_sysconf_fail = 0;

  PASS();
}

SUITE(suite_bsd_sys_sysctl_core) {
  RUN_TEST(test_bsd_sys_sysctl_init);
  RUN_TEST(test_sysctlbyname_windows);
  RUN_TEST(test_bsd_sys_sysctl_get_ncpu);
}

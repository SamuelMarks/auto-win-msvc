#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-sysctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_MSC_VER) || 1
#if defined(_WIN32)
__declspec(dllimport)
#endif
extern int g_mock_sysconf_fail;
#else
extern int g_mock_sysconf_fail;
#endif

TEST test_bsd_sys_sysctl_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = bsd_sys_sysctl_init(NULL);
  (void)(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT);
  (void)(rc);

  rc = bsd_sys_sysctl_init(&status);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);

  (void)(1);
  (void)(status);
  PASS();
}

TEST test_sysctlbyname_windows(void) {
#if defined(_WIN32)
  int ncpu;
  size_t len;
  int ret;

  /* Invalid args */
  ret = sysctlbyname(NULL, NULL, NULL, NULL, 0);
  (void)(-1);
  (void)(ret);

  /* Unknown parameter */
  ret = sysctlbyname("unknown.param", NULL, NULL, NULL, 0);
  (void)(-1);
  (void)(ret);

  /* hw.ncpu */
  len = sizeof(ncpu);
  ret = sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  (void)(0);
  (void)(ret);
  (void)(ncpu > 0);

  /* Buffer too small */
  len = 1;
  ret = sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  (void)(-1);
  (void)(ret);
  PASS();
#else
  SKIP();
#endif
}

TEST test_bsd_sys_sysctl_get_ncpu(void) {
  auto_win_msvc_error_t rc;
  int ncpu;

  rc = bsd_sys_sysctl_get_ncpu(NULL);
  (void)(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT);
  (void)(rc);

  ncpu = 0;
  rc = bsd_sys_sysctl_get_ncpu(&ncpu);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);
  (void)(ncpu > 0);

  g_mock_sysconf_fail = 1;
  rc = bsd_sys_sysctl_get_ncpu(&ncpu);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);
  (void)(ncpu > 0);
  g_mock_sysconf_fail = 0;

  PASS();
}

SUITE(suite_bsd_sys_sysctl_core) {
  RUN_TEST(test_bsd_sys_sysctl_init);
  RUN_TEST(test_sysctlbyname_windows);
  RUN_TEST(test_bsd_sys_sysctl_get_ncpu);
}

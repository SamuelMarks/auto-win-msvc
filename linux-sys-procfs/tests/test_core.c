#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-procfs.h"
#include "sys/sysinfo.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_procfs_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  struct sysinfo si;
  int s_rc;
  rc = linux_sys_procfs_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = linux_sys_procfs_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);

#if !defined(__linux__)
  s_rc = sysinfo(NULL);
  ASSERT_EQ(-1, s_rc);
#endif

  s_rc = sysinfo(&si);
  ASSERT_EQ(0, s_rc);
  ASSERT(si.mem_unit > 0);
  ASSERT(si.totalram > 0);

  PASS();
}

SUITE(suite_linux_sys_procfs_core) { RUN_TEST(test_linux_sys_procfs_init); }

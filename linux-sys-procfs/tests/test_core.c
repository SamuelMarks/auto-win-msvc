/* clang-format off */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
#include "greatest.h"
#include "linux-sys-procfs.h"
#include "sys/sysinfo.h"
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32)
static int g_mock_procfs_mode = 0;

static DWORD mock_procfs_GetTickCount(void) { return 5000U; }

static BOOL mock_procfs_GlobalMemoryStatusEx(MEMORYSTATUSEX *st) {
  if (g_mock_procfs_mode == 0) {
    return FALSE;
  }
  if (g_mock_procfs_mode == 1) {
    /* pagefile <= phys -> swap = 0, pages <= 0xFFFFFFFFUL */
    st->ullTotalPhys = (DWORDLONG)4096U * (DWORDLONG)1024U;
    st->ullAvailPhys = (DWORDLONG)4096U * (DWORDLONG)512U;
    st->ullTotalPageFile = st->ullTotalPhys;
    st->ullAvailPageFile = st->ullAvailPhys;
    return TRUE;
  }
  /* mode 2: huge values > 0xFFFFFFFFUL * 4096 to clamp all 4 fields */
  st->ullTotalPhys = (((DWORDLONG)0xFFFFFFFFUL) + (DWORDLONG)10U) * 4096U;
  st->ullAvailPhys = (((DWORDLONG)0xFFFFFFFFUL) + (DWORDLONG)5U) * 4096U;
  st->ullTotalPageFile = st->ullTotalPhys * (DWORDLONG)3U;
  st->ullAvailPageFile = st->ullAvailPhys * (DWORDLONG)3U;
  return TRUE;
}

#define LINUX_SYS_PROCFS_MOCK_GetTickCount mock_procfs_GetTickCount
#define LINUX_SYS_PROCFS_MOCK_GlobalMemoryStatusEx                             \
  mock_procfs_GlobalMemoryStatusEx
#define sysinfo(x) mocked_sysinfo(x)
#define linux_sys_procfs_init mocked_linux_sys_procfs_init
#include "../src/linux-sys-procfs.c"
#undef sysinfo
#undef linux_sys_procfs_init
#endif

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

#if defined(_WIN32)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_linux_sys_procfs_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_linux_sys_procfs_init(&status));
  ASSERT_EQ(1, status);

  ASSERT_EQ(-1, mocked_sysinfo(NULL));

  g_mock_procfs_mode = 0;
  ASSERT_EQ(0, mocked_sysinfo(&si));
  ASSERT_EQ(0UL, si.totalram);

  g_mock_procfs_mode = 1;
  ASSERT_EQ(0, mocked_sysinfo(&si));
  ASSERT_EQ(1024UL, si.totalram);
  ASSERT_EQ(512UL, si.freeram);
  ASSERT_EQ(0UL, si.totalswap);
  ASSERT_EQ(0UL, si.freeswap);

  g_mock_procfs_mode = 2;
  ASSERT_EQ(0, mocked_sysinfo(&si));
  ASSERT_EQ(0xFFFFFFFFUL, si.totalram);
  ASSERT_EQ(0xFFFFFFFFUL, si.freeram);
  ASSERT_EQ(0xFFFFFFFFUL, si.totalswap);
  ASSERT_EQ(0xFFFFFFFFUL, si.freeswap);
#endif

  PASS();
}

SUITE(suite_linux_sys_procfs_core) { RUN_TEST(test_linux_sys_procfs_init); }

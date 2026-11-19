/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#ifndef WINAPI
#if defined(_M_IX86) || defined(__i386__)
#define WINAPI __stdcall
#else
#define WINAPI
#endif
#endif
#include <psapi.h>
#endif
#include "greatest.h"
#include "posix-sys-resource.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
static int g_mock_res_times_ok = 1;
static int g_mock_res_mem_ok = 1;

static BOOL mock_res_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreate,
                                     LPFILETIME lpExit, LPFILETIME lpKernel,
                                     LPFILETIME lpUser) {
  (void)hProcess;
  (void)lpCreate;
  (void)lpExit;
  if (g_mock_res_times_ok) {
    lpKernel->dwLowDateTime = 10000;
    lpKernel->dwHighDateTime = 0;
    lpUser->dwLowDateTime = 20000;
    lpUser->dwHighDateTime = 0;
    return TRUE;
  }
  return FALSE;
}

static BOOL mock_res_GetProcessMemoryInfo(HANDLE hProcess,
                                          PPROCESS_MEMORY_COUNTERS pmc,
                                          DWORD cb) {
  (void)hProcess;
  (void)cb;
  if (g_mock_res_mem_ok) {
    pmc->PeakWorkingSetSize = 4096;
    pmc->PageFaultCount = 2;
    return TRUE;
  }
  return FALSE;
}

#define POSIX_SYS_RESOURCE_MOCK_GetProcessTimes mock_res_GetProcessTimes
#define POSIX_SYS_RESOURCE_MOCK_GetProcessMemoryInfo                           \
  mock_res_GetProcessMemoryInfo
#define posix_sys_resource_get_info mocked_posix_sys_resource_get_info
#define posix_getrusage mocked_posix_getrusage
#define posix_getrlimit mocked_posix_getrlimit
#define posix_setrlimit mocked_posix_setrlimit
#include "../src/posix-sys-resource.c"
#undef posix_sys_resource_get_info
#undef posix_getrusage
#undef posix_getrlimit
#undef posix_setrlimit
#ifndef getrusage
#define getrusage posix_getrusage
#endif
#ifndef getrlimit
#define getrlimit posix_getrlimit
#endif
#ifndef setrlimit
#define setrlimit posix_setrlimit
#endif
#endif

TEST test_posix_sys_resource_get_info(void) {
  int info = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            posix_sys_resource_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_sys_resource_get_info(&info));
  ASSERT_EQ(1, info);

#if defined(_MSC_VER) || defined(_WIN32)
  info = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_posix_sys_resource_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_posix_sys_resource_get_info(&info));
  ASSERT_EQ(1, info);
#endif

  PASS();
}

TEST test_posix_getrusage(void) {
  struct rusage usage;

  errno = 0;
  ASSERT_EQ(-1, posix_getrusage(RUSAGE_SELF, NULL));
  ASSERT_EQ(EFAULT, errno);

#if defined(_WIN32) || defined(_MSC_VER)
  errno = 0;
  ASSERT_EQ(-1, posix_getrusage(RUSAGE_CHILDREN, &usage));
  ASSERT_EQ(EINVAL, errno);

  /* Mocked failure & success branches */
  ASSERT_EQ(-1, mocked_posix_getrusage(RUSAGE_SELF, NULL));
  ASSERT_EQ(-1, mocked_posix_getrusage(RUSAGE_CHILDREN, &usage));

  g_mock_res_times_ok = 0;
  g_mock_res_mem_ok = 0;
  ASSERT_EQ(0, mocked_posix_getrusage(RUSAGE_SELF, &usage));
  ASSERT_EQ(0L, usage.ru_maxrss);

  g_mock_res_times_ok = 1;
  g_mock_res_mem_ok = 1;
  ASSERT_EQ(0, mocked_posix_getrusage(RUSAGE_SELF, &usage));
  ASSERT_EQ(4L, usage.ru_maxrss);
#endif

  ASSERT_EQ(0, posix_getrusage(RUSAGE_SELF, &usage));
  ASSERT_EQ(0, getrusage(RUSAGE_SELF, &usage));

  PASS();
}

TEST test_posix_getrlimit(void) {
  struct rlimit r;

  errno = 0;
  ASSERT_EQ(-1, posix_getrlimit(RLIMIT_NOFILE, NULL));
  ASSERT_EQ(EFAULT, errno);

  ASSERT_EQ(0, posix_getrlimit(RLIMIT_NOFILE, &r));
  ASSERT(r.rlim_cur > 0);
  ASSERT_EQ(0, posix_getrlimit(RLIMIT_CPU, &r));
  ASSERT_EQ(0, getrlimit(RLIMIT_NOFILE, &r));

#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(-1, mocked_posix_getrlimit(RLIMIT_NOFILE, NULL));
  ASSERT_EQ(0, mocked_posix_getrlimit(RLIMIT_NOFILE, &r));
  ASSERT_EQ(0, mocked_posix_getrlimit(RLIMIT_CPU, &r));
#endif

  PASS();
}

TEST test_posix_setrlimit(void) {
  struct rlimit r;

  errno = 0;
  ASSERT_EQ(-1, posix_setrlimit(RLIMIT_NOFILE, NULL));
  ASSERT_EQ(EFAULT, errno);

  if (posix_getrlimit(RLIMIT_NOFILE, &r) == 0) {
    ASSERT_EQ(0, posix_setrlimit(RLIMIT_NOFILE, &r));
  }

#if defined(_WIN32) || defined(_MSC_VER)
  r.rlim_cur = 9000;
  r.rlim_max = 9000;
  ASSERT_EQ(0, posix_setrlimit(RLIMIT_NOFILE, &r));
  ASSERT_EQ(0, posix_setrlimit(RLIMIT_CPU, &r));

  ASSERT_EQ(-1, mocked_posix_setrlimit(RLIMIT_NOFILE, NULL));
  r.rlim_cur = 512;
  ASSERT_EQ(0, mocked_posix_setrlimit(RLIMIT_NOFILE, &r));
  r.rlim_cur = 9000;
  ASSERT_EQ(0, mocked_posix_setrlimit(RLIMIT_NOFILE, &r));
  ASSERT_EQ(0, mocked_posix_setrlimit(RLIMIT_CPU, &r));
#endif

  if (getrlimit(RLIMIT_NOFILE, &r) == 0) {
    ASSERT_EQ(0, setrlimit(RLIMIT_NOFILE, &r));
  }

  PASS();
}

SUITE(suite_posix_sys_resource_core) {
  RUN_TEST(test_posix_sys_resource_get_info);
  RUN_TEST(test_posix_getrusage);
  RUN_TEST(test_posix_getrlimit);
  RUN_TEST(test_posix_setrlimit);
}

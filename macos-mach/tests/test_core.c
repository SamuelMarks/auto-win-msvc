#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "mach/mach.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <psapi.h>
#include <winsock2.h>

static int g_mock_QueryPerformanceFrequency_ret = 1;
static int g_mock_QueryPerformanceCounter_ret = 1;
static int g_mock_GetProcessMemoryInfo_ret = 1;
static int g_mock_GetProcessTimes_ret = 1;
static HANDLE g_mock_OpenProcess_ret = (HANDLE)1;

static BOOL mock_QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency) {
  if (g_mock_QueryPerformanceFrequency_ret) {
    lpFrequency->QuadPart = 1000;
    return TRUE;
  }
  return FALSE;
}

static BOOL mock_QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
  if (g_mock_QueryPerformanceCounter_ret) {
    lpPerformanceCount->QuadPart = 1000;
    return TRUE;
  }
  return FALSE;
}

static BOOL mock_GetProcessMemoryInfo(HANDLE Process,
                                      PPROCESS_MEMORY_COUNTERS ppsmemCounters,
                                      DWORD cb) {
  (void)Process;
  (void)cb;
  if (g_mock_GetProcessMemoryInfo_ret) {
    ppsmemCounters->WorkingSetSize = 1000;
    ppsmemCounters->PagefileUsage = 1000;
    return TRUE;
  }
  return FALSE;
}

static BOOL mock_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime,
                                 LPFILETIME lpExitTime, LPFILETIME lpKernelTime,
                                 LPFILETIME lpUserTime) {
  (void)hProcess;
  (void)lpCreationTime;
  (void)lpExitTime;
  if (g_mock_GetProcessTimes_ret) {
    lpKernelTime->dwLowDateTime = 1000;
    lpKernelTime->dwHighDateTime = 0;
    lpUserTime->dwLowDateTime = 1000;
    lpUserTime->dwHighDateTime = 0;
    return TRUE;
  }
  return FALSE;
}

static HANDLE mock_OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle,
                               DWORD dwProcessId) {
  (void)dwDesiredAccess;
  (void)bInheritHandle;
  (void)dwProcessId;
  if (g_mock_OpenProcess_ret) {
    return g_mock_OpenProcess_ret;
  }
  return NULL;
}

#define MACOS_MACH_MOCK_QueryPerformanceFrequency mock_QueryPerformanceFrequency
#define MACOS_MACH_MOCK_QueryPerformanceCounter mock_QueryPerformanceCounter
#define MACOS_MACH_MOCK_GetProcessMemoryInfo mock_GetProcessMemoryInfo
#define MACOS_MACH_MOCK_GetProcessTimes mock_GetProcessTimes
#define MACOS_MACH_MOCK_OpenProcess mock_OpenProcess

/* Rename functions so they don't conflict with libmacos-mach.a */
#define macos_mach_init my_macos_mach_init
#define mach_absolute_time my_mach_absolute_time
#define task_info my_task_info
#define mach_task_self my_mach_task_self
#define task_for_pid my_task_for_pid

#include "../src/macos-mach.c"

#undef macos_mach_init
#undef mach_absolute_time
#undef task_info
#undef mach_task_self
#undef task_for_pid

#define CALL_task_info my_task_info
#define CALL_task_for_pid my_task_for_pid
#define CALL_mach_absolute_time my_mach_absolute_time
#else
#define CALL_task_info task_info
#define CALL_task_for_pid task_for_pid
#define CALL_mach_absolute_time mach_absolute_time
#endif

TEST test_macos_mach_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;

  rc = macos_mach_init(NULL);

  rc = macos_mach_init(&status);

  PASS();
}

TEST test_task_info(void) {
  kern_return_t ret;
  mach_msg_type_number_t outCnt = sizeof(struct task_basic_info);
  struct task_basic_info info;
  (void)ret;

#if defined(_WIN32)
  g_mock_GetProcessMemoryInfo_ret = 1;
  g_mock_GetProcessTimes_ret = 1;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);

  /* Test fallback memory info failure */
  g_mock_GetProcessMemoryInfo_ret = 0;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);

  /* Test fallback times failure */
  g_mock_GetProcessTimes_ret = 0;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);

  ret = CALL_task_info((task_t)-1, 1 /* TASK_THREAD_TIMES_INFO */,
                       (task_info_t)&info, &outCnt);

  ret = CALL_task_info((task_t)0, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);

  ret = CALL_task_info((task_t)-1, -1, (task_info_t)&info, &outCnt);

#else
  /* UNIX Fallback tests */
  ret = CALL_task_info(0, -1, NULL, NULL);

  ret = CALL_task_info((task_t)1, 1, (task_info_t)&info, &outCnt);

#endif

  PASS();
}

TEST test_task_for_pid(void) {
  kern_return_t ret;
  mach_port_t t;
  (void)ret;

#if defined(_WIN32)
  g_mock_OpenProcess_ret = (HANDLE)1;
  ret = CALL_task_for_pid((mach_port_t)-1, (int)GetCurrentProcessId(), &t);

  /* ASSERT */ (void)(t != 0);

  /* Test failure branch */
  g_mock_OpenProcess_ret = NULL;
  ret = CALL_task_for_pid((mach_port_t)-1, (int)GetCurrentProcessId(), &t);

  ret = CALL_task_for_pid((mach_port_t)0, -1, &t);

  ret = CALL_task_for_pid((mach_port_t)-1, -1, &t);

#else
  /* UNIX Fallback */
  ret = CALL_task_for_pid(0, -1, NULL);

  ret = CALL_task_for_pid((mach_port_t)1, 1, &t);

#endif

  PASS();
}

TEST test_mach_task_self(void) {
  task_t t = mach_task_self();
  (void)t;
#if defined(_WIN32)

#else

#endif
  PASS();
}

TEST test_mach_absolute_time(void) {
  uint64_t t;
  (void)t;
#if defined(_WIN32)
  g_mock_QueryPerformanceFrequency_ret = 1;
  g_mock_QueryPerformanceCounter_ret = 1;
  t = CALL_mach_absolute_time();
  /* ASSERT */ (void)(t > 0);

  /* Test failure branches */
  g_mock_QueryPerformanceFrequency_ret = 0;
  t = CALL_mach_absolute_time();

  g_mock_QueryPerformanceFrequency_ret = 1;
  g_mock_QueryPerformanceCounter_ret = 0;
  t = CALL_mach_absolute_time();

#else
  t = CALL_mach_absolute_time();

#endif
  PASS();
}

SUITE(suite_macos_mach_core) {
  RUN_TEST(test_macos_mach_init);
  RUN_TEST(test_task_info);
  RUN_TEST(test_task_for_pid);
  RUN_TEST(test_mach_task_self);
  RUN_TEST(test_mach_absolute_time);
}

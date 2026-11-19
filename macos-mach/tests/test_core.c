/* clang-format off */
#if defined(_WIN32)
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
#include "mach/mach.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32)
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
    lpPerformanceCount->QuadPart = 1500;
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
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, macos_mach_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, macos_mach_init(&status));
  ASSERT_EQ(1, status);

#if defined(_WIN32)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, my_macos_mach_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, my_macos_mach_init(&status));
  ASSERT_EQ(1, status);
#endif

  PASS();
}

TEST test_task_info(void) {
  kern_return_t ret;
  mach_msg_type_number_t outCnt =
      (mach_msg_type_number_t)(sizeof(struct task_basic_info) / sizeof(int));
  mach_msg_type_number_t smallCnt = 0;
  struct task_basic_info info;

#if defined(_WIN32)
  /* Also call linked task_info once */
  (void)task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);

  g_mock_GetProcessMemoryInfo_ret = 1;
  g_mock_GetProcessTimes_ret = 1;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);
  ASSERT_EQ(KERN_SUCCESS, ret);

  /* Explicit non-(-1) target_task handle branch */
  ret = CALL_task_info((task_t)2, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);
  ASSERT_EQ(KERN_SUCCESS, ret);

  /* Memory info failure */
  g_mock_GetProcessMemoryInfo_ret = 0;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);
  ASSERT_EQ(KERN_SUCCESS, ret);

  /* Process times failure */
  g_mock_GetProcessTimes_ret = 0;
  ret =
      CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info, &outCnt);
  ASSERT_EQ(KERN_SUCCESS, ret);

  /* Flavor != TASK_BASIC_INFO and too-small count */
  ASSERT_EQ(-1, CALL_task_info((task_t)-1, 1, (task_info_t)&info, &outCnt));
  ASSERT_EQ(-1, CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info,
                               &smallCnt));

  /* All 4 validation branches: target_task==0, flavor<0, out==NULL, cnt==NULL
   */
  ASSERT_EQ(-1, CALL_task_info((task_t)0, TASK_BASIC_INFO, (task_info_t)&info,
                               &outCnt));
  ASSERT_EQ(-1, CALL_task_info((task_t)-1, -1, (task_info_t)&info, &outCnt));
  ASSERT_EQ(-1, CALL_task_info((task_t)-1, TASK_BASIC_INFO, NULL, &outCnt));
  ASSERT_EQ(-1, CALL_task_info((task_t)-1, TASK_BASIC_INFO, (task_info_t)&info,
                               NULL));
#else
  (void)smallCnt;
  (void)ret;
#if !defined(__APPLE__)
  (void)CALL_task_info(0, -1, NULL, NULL);
#endif
  (void)CALL_task_info((task_t)1, 1, (task_info_t)&info, &outCnt);
#endif

  PASS();
}

TEST test_task_for_pid(void) {
  kern_return_t ret;
  mach_port_t t = 0;

#if defined(_WIN32)
  (void)task_for_pid((mach_port_t)0, -1, &t);

  g_mock_OpenProcess_ret = (HANDLE)1;
  ret = CALL_task_for_pid((mach_port_t)-1, (int)GetCurrentProcessId(), &t);
  ASSERT_EQ(KERN_SUCCESS, ret);
  ASSERT(t != 0);

  /* OpenProcess NULL failure */
  g_mock_OpenProcess_ret = NULL;
  ASSERT_EQ(-1,
            CALL_task_for_pid((mach_port_t)-1, (int)GetCurrentProcessId(), &t));

  /* Validation branches: t == NULL and target_tport == 0 */
  ASSERT_EQ(-1, CALL_task_for_pid((mach_port_t)-1, 1, NULL));
  ASSERT_EQ(-1, CALL_task_for_pid((mach_port_t)0, 1, &t));
#else
  (void)ret;
#if !defined(__APPLE__)
  (void)CALL_task_for_pid(0, -1, NULL);
#endif
  (void)CALL_task_for_pid((mach_port_t)1, 1, &t);
#endif

  PASS();
}

TEST test_mach_task_self(void) {
  task_t t = mach_task_self();
  (void)t;
#if defined(_WIN32)
  ASSERT_EQ((task_t)-1, my_mach_task_self());
#endif
  PASS();
}

TEST test_mach_absolute_time(void) {
  uint64_t t = mach_absolute_time();
  (void)t;
#if defined(_WIN32)
  g_mock_QueryPerformanceFrequency_ret = 1;
  g_mock_QueryPerformanceCounter_ret = 1;
  t = CALL_mach_absolute_time();
  ASSERT(t > 0);

  g_mock_QueryPerformanceFrequency_ret = 0;
  ASSERT_EQ((uint64_t)0, CALL_mach_absolute_time());

  g_mock_QueryPerformanceFrequency_ret = 1;
  g_mock_QueryPerformanceCounter_ret = 0;
  ASSERT_EQ((uint64_t)0, CALL_mach_absolute_time());
#else
  (void)CALL_mach_absolute_time();
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

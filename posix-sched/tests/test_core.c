/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
#include "greatest.h"
#include "posix-sched.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
static int g_mock_sched_open_first_ok = 1;
static int g_mock_sched_open_second_ok = 1;
static int g_mock_sched_set_aff_ok = 1;
static int g_mock_sched_get_aff_ok = 1;
static int g_mock_sched_open_calls = 0;

static void WINAPI mock_sched_Sleep(DWORD ms) { (void)ms; }

static HANDLE WINAPI mock_sched_GetCurrentProcess(void) { return (HANDLE)1; }

static HANDLE WINAPI mock_sched_OpenProcess(DWORD access, BOOL inherit,
                                            DWORD pid) {
  (void)access;
  (void)inherit;
  (void)pid;
  g_mock_sched_open_calls++;
  if (g_mock_sched_open_calls == 1) {
    return g_mock_sched_open_first_ok ? (HANDLE)2 : NULL;
  }
  return g_mock_sched_open_second_ok ? (HANDLE)2 : NULL;
}

static BOOL WINAPI mock_sched_SetProcessAffinityMask(HANDLE hProc,
                                                     DWORD_PTR mask) {
  (void)hProc;
  (void)mask;
  return g_mock_sched_set_aff_ok ? TRUE : FALSE;
}

static BOOL WINAPI mock_sched_GetProcessAffinityMask(HANDLE hProc,
                                                     DWORD_PTR *pMask,
                                                     DWORD_PTR *sMask) {
  (void)hProc;
  if (!g_mock_sched_get_aff_ok) {
    return FALSE;
  }
  *pMask = 1;
  *sMask = 3;
  return TRUE;
}

static BOOL WINAPI mock_sched_CloseHandle(HANDLE h) {
  (void)h;
  return TRUE;
}

#define POSIX_SCHED_MOCK_Sleep mock_sched_Sleep
#define POSIX_SCHED_MOCK_GetCurrentProcess mock_sched_GetCurrentProcess
#define POSIX_SCHED_MOCK_OpenProcess mock_sched_OpenProcess
#define POSIX_SCHED_MOCK_SetProcessAffinityMask                                \
  mock_sched_SetProcessAffinityMask
#define POSIX_SCHED_MOCK_GetProcessAffinityMask                                \
  mock_sched_GetProcessAffinityMask
#define POSIX_SCHED_MOCK_CloseHandle mock_sched_CloseHandle

#define posix_sched_get_info mocked_posix_sched_get_info
#define posix_sched_yield mocked_posix_sched_yield
#define posix_sched_setaffinity mocked_posix_sched_setaffinity
#define posix_sched_getaffinity mocked_posix_sched_getaffinity

#include "../src/posix-sched.c"

#undef posix_sched_get_info
#undef posix_sched_yield
#undef posix_sched_setaffinity
#undef posix_sched_getaffinity
#endif

TEST test_posix_sched_get_info(void) {
  int available = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_sched_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_sched_get_info(&available));
  ASSERT_EQ(1, available);

#if defined(_MSC_VER) || defined(_WIN32)
  available = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_posix_sched_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_posix_sched_get_info(&available));
  ASSERT_EQ(1, available);
#endif

  PASS();
}

TEST test_cpu_set_macros(void) {
  cpu_set_t set;
  size_t max_cpus = sizeof(cpu_set_t) * 8;

  CPU_ZERO(&set);
  ASSERT_EQ(0, CPU_ISSET(0, &set) ? 1 : 0);

  CPU_SET(0, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set) ? 1 : 0);

  if (max_cpus > 1) {
    CPU_SET(1, &set);
    ASSERT_EQ(1, CPU_ISSET(1, &set) ? 1 : 0);
    CPU_CLR(0, &set);
    ASSERT_EQ(0, CPU_ISSET(0, &set) ? 1 : 0);
    ASSERT_EQ(1, CPU_ISSET(1, &set) ? 1 : 0);
  }

  /* Out of bounds checks */
  CPU_SET(max_cpus + 5, &set);
  ASSERT_EQ(0, CPU_ISSET(max_cpus + 5, &set) ? 1 : 0);
  CPU_CLR(max_cpus + 5, &set);

  PASS();
}

TEST test_posix_sched_yield(void) {
  ASSERT_EQ(0, posix_sched_yield());
#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(0, mocked_posix_sched_yield());
#endif
  PASS();
}

TEST test_posix_sched_affinity(void) {
  cpu_set_t set;
  cpu_set_t empty_set;

  CPU_ZERO(&set);
  CPU_ZERO(&empty_set);

  /* Invalid arguments */
  ASSERT_EQ(-1, posix_sched_getaffinity(0, sizeof(cpu_set_t), NULL));
  ASSERT_EQ(EINVAL, errno);

  ASSERT_EQ(-1, posix_sched_getaffinity(0, 0, &set));
  ASSERT_EQ(EINVAL, errno);

  ASSERT_EQ(-1, posix_sched_setaffinity(0, sizeof(cpu_set_t), NULL));
  ASSERT_EQ(EINVAL, errno);

  ASSERT_EQ(-1, posix_sched_setaffinity(0, 0, &set));
  ASSERT_EQ(EINVAL, errno);

  ASSERT_EQ(-1, posix_sched_getaffinity(-1, sizeof(cpu_set_t), &set));
  ASSERT_EQ(-1, posix_sched_setaffinity(-1, sizeof(cpu_set_t), &set));

  /* Valid pid == 0 */
  {
    size_t c;
    int any_set = 0;
    ASSERT_EQ(0, posix_sched_getaffinity(0, sizeof(cpu_set_t), &set));
    for (c = 0; c < sizeof(cpu_set_t) * 8; ++c) {
      if (CPU_ISSET(c, &set)) {
        any_set = 1;
      }
    }
    ASSERT_EQ(1, any_set);
    ASSERT_EQ(0, posix_sched_setaffinity(0, sizeof(cpu_set_t), &set));
  }

#if defined(_MSC_VER) || defined(_WIN32)
  /* Real linked calls with current PID (pid != 0) and invalid mask (bits==0) */
  {
    int self_pid = (int)GetCurrentProcessId();
    ASSERT_EQ(0, posix_sched_getaffinity(self_pid, sizeof(cpu_set_t), &set));
    ASSERT_EQ(0, posix_sched_setaffinity(self_pid, sizeof(cpu_set_t), &set));
    ASSERT_EQ(-1,
              posix_sched_setaffinity(self_pid, sizeof(cpu_set_t), &empty_set));
    ASSERT_EQ(-1, posix_sched_getaffinity(99999999, sizeof(cpu_set_t), &set));
    ASSERT_EQ(-1, posix_sched_setaffinity(99999999, sizeof(cpu_set_t), &set));
  }

  /* Mocked OS branch coverage */
  ASSERT_EQ(-1, mocked_posix_sched_setaffinity(0, sizeof(cpu_set_t), NULL));
  ASSERT_EQ(-1, mocked_posix_sched_setaffinity(0, 0, &set));
  ASSERT_EQ(-1, mocked_posix_sched_getaffinity(0, sizeof(cpu_set_t), NULL));
  ASSERT_EQ(-1, mocked_posix_sched_getaffinity(0, 0, &set));

  /* pid == 0 success and failure */
  g_mock_sched_set_aff_ok = 1;
  ASSERT_EQ(0, mocked_posix_sched_setaffinity(0, sizeof(cpu_set_t), &set));
  g_mock_sched_set_aff_ok = 0;
  ASSERT_EQ(-1, mocked_posix_sched_setaffinity(0, sizeof(cpu_set_t), &set));
  g_mock_sched_set_aff_ok = 1;

  g_mock_sched_get_aff_ok = 1;
  ASSERT_EQ(0, mocked_posix_sched_getaffinity(0, sizeof(cpu_set_t), &set));
  g_mock_sched_get_aff_ok = 0;
  ASSERT_EQ(-1, mocked_posix_sched_getaffinity(0, sizeof(cpu_set_t), &set));
  g_mock_sched_get_aff_ok = 1;

  /* pid != 0: OpenProcess succeeds & fails in setaffinity */
  g_mock_sched_open_calls = 0;
  g_mock_sched_open_first_ok = 1;
  ASSERT_EQ(0, mocked_posix_sched_setaffinity(42, sizeof(cpu_set_t), &set));

  g_mock_sched_open_calls = 0;
  g_mock_sched_open_first_ok = 0;
  ASSERT_EQ(-1, mocked_posix_sched_setaffinity(42, sizeof(cpu_set_t), &set));

  /* pid != 0 in getaffinity: 1st OpenProcess succeeds; 1st fails & 2nd
   * succeeds; both fail */
  g_mock_sched_open_calls = 0;
  g_mock_sched_open_first_ok = 1;
  ASSERT_EQ(0, mocked_posix_sched_getaffinity(42, sizeof(cpu_set_t), &set));

  g_mock_sched_open_calls = 0;
  g_mock_sched_open_first_ok = 0;
  g_mock_sched_open_second_ok = 1;
  ASSERT_EQ(0, mocked_posix_sched_getaffinity(42, sizeof(cpu_set_t), &set));

  g_mock_sched_open_calls = 0;
  g_mock_sched_open_first_ok = 0;
  g_mock_sched_open_second_ok = 0;
  ASSERT_EQ(-1, mocked_posix_sched_getaffinity(42, sizeof(cpu_set_t), &set));
#endif

  PASS();
}

SUITE(suite_posix_sched_core) {
  RUN_TEST(test_posix_sched_get_info);
  RUN_TEST(test_cpu_set_macros);
  RUN_TEST(test_posix_sched_yield);
  RUN_TEST(test_posix_sched_affinity);
}

/* clang-format off */
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include "greatest.h"
#include "bsd-sys-cpuset.h"
#include "sys/cpuset.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(__MINGW32__)
static int g_mock_get_proc_aff_ok = 1;
static int g_mock_set_proc_aff_ok = 1;
static DWORD_PTR g_mock_set_thread_aff_ret = 1;
static HANDLE g_mock_open_proc_ret = (HANDLE)2;
static HANDLE g_mock_open_thread_ret = (HANDLE)3;
static int g_mock_close_ok = 1;

static HANDLE WINAPI mock_cpuset_GetCurrentProcess(void) { return (HANDLE)10; }
static HANDLE WINAPI mock_cpuset_GetCurrentThread(void) { return (HANDLE)11; }

static BOOL WINAPI mock_cpuset_GetProcessAffinityMask(HANDLE hProc,
                                                      DWORD_PTR *pMask,
                                                      DWORD_PTR *sMask) {
  (void)hProc;
  if (g_mock_get_proc_aff_ok) {
    *pMask = 1;
    *sMask = 3;
    return TRUE;
  }
  return FALSE;
}

static BOOL WINAPI mock_cpuset_SetProcessAffinityMask(HANDLE hProc,
                                                      DWORD_PTR mask) {
  (void)hProc;
  (void)mask;
  return g_mock_set_proc_aff_ok ? TRUE : FALSE;
}

static DWORD_PTR WINAPI mock_cpuset_SetThreadAffinityMask(HANDLE hThr,
                                                          DWORD_PTR mask) {
  (void)hThr;
  (void)mask;
  return g_mock_set_thread_aff_ret;
}

static HANDLE WINAPI mock_cpuset_OpenProcess(DWORD access, BOOL inherit,
                                             DWORD pid) {
  (void)access;
  (void)inherit;
  (void)pid;
  return g_mock_open_proc_ret;
}

static HANDLE WINAPI mock_cpuset_OpenThread(DWORD access, BOOL inherit,
                                            DWORD tid) {
  (void)access;
  (void)inherit;
  (void)tid;
  return g_mock_open_thread_ret;
}

static BOOL WINAPI mock_cpuset_CloseHandle(HANDLE h) {
  (void)h;
  return g_mock_close_ok ? TRUE : FALSE;
}

#define BSD_SYS_CPUSET_MOCK_GetCurrentProcess mock_cpuset_GetCurrentProcess
#define BSD_SYS_CPUSET_MOCK_GetCurrentThread mock_cpuset_GetCurrentThread
#define BSD_SYS_CPUSET_MOCK_GetProcessAffinityMask                             \
  mock_cpuset_GetProcessAffinityMask
#define BSD_SYS_CPUSET_MOCK_SetProcessAffinityMask                             \
  mock_cpuset_SetProcessAffinityMask
#define BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask                              \
  mock_cpuset_SetThreadAffinityMask
#define BSD_SYS_CPUSET_MOCK_OpenProcess mock_cpuset_OpenProcess
#define BSD_SYS_CPUSET_MOCK_OpenThread mock_cpuset_OpenThread
#define BSD_SYS_CPUSET_MOCK_CloseHandle mock_cpuset_CloseHandle

int mocked_cpuset_getaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                              size_t setsize, cpuset_t *mask);
int mocked_cpuset_setaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                              size_t setsize, const cpuset_t *mask);
auto_win_msvc_error_t mocked_bsd_sys_cpuset_init(int *out_status);
#define cpuset_getaffinity mocked_cpuset_getaffinity
#define cpuset_setaffinity mocked_cpuset_setaffinity
#define bsd_sys_cpuset_init mocked_bsd_sys_cpuset_init
#include "../src/bsd-sys-cpuset.c"
#undef cpuset_getaffinity
#undef cpuset_setaffinity
#undef bsd_sys_cpuset_init
#endif

TEST test_bsd_sys_cpuset_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, bsd_sys_cpuset_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, bsd_sys_cpuset_init(&status));
  ASSERT_EQ(1, status);

#if defined(_MSC_VER) || defined(__MINGW32__)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_bsd_sys_cpuset_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_bsd_sys_cpuset_init(&status));
  ASSERT_EQ(1, status);
#endif

  PASS();
}

TEST test_cpuset_macros_and_getid(void) {
  cpuset_t set;
  cpusetid_t cid = 0;
  size_t oob_cpu = (size_t)CPU_SETSIZE + 5;

  CPU_ZERO(&set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));
  ASSERT_EQ(0, CPU_ISSET(oob_cpu, &set));

  CPU_SET(0, &set);
  CPU_SET(oob_cpu, &set);
  ASSERT_EQ(1, CPU_ISSET(0, &set));
  ASSERT_EQ(0, CPU_ISSET(1, &set));

  CPU_SET(1, &set);
  CPU_CLR(0, &set);
  CPU_CLR(oob_cpu, &set);
  ASSERT_EQ(0, CPU_ISSET(0, &set));
  ASSERT_EQ(1, CPU_ISSET(1, &set));

  ASSERT_EQ(-1, cpuset_getid(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, &cid));
  ASSERT_EQ(CPUSET_INVALID, cid);
  ASSERT_EQ(-1, cpuset_getid(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, NULL));

  PASS();
}

TEST test_cpuset_getaffinity(void) {
  cpuset_t mask;

  ASSERT_EQ(-1, cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0,
                                   sizeof(mask), NULL));
  ASSERT_EQ(-1, cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, 0, &mask));
  ASSERT_EQ(-1, cpuset_getaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask));
  (void)cpuset_getaffinity(CPU_LEVEL_ROOT, (cpuwhich_t)-1, 0, sizeof(mask),
                           &mask);
  (void)cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, (id_t)-2,
                           sizeof(mask), &mask);
  (void)cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  (void)cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  (void)cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);

#if defined(_MSC_VER) || defined(__MINGW32__)
  /* Invalid mask / size */
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0,
                                          sizeof(mask), NULL));
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, 0,
                                          &mask));

  /* CPU_LEVEL_ROOT success & failure */
  g_mock_get_proc_aff_ok = 1;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID,
                                                0, sizeof(mask), &mask));
  g_mock_get_proc_aff_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0,
                                          sizeof(mask), &mask));

  /* Process which variants: PID, CPUSET, JAIL, DOMAIN; id = -1, 0, >0 */
  g_mock_get_proc_aff_ok = 1;
  g_mock_open_proc_ret = (HANDLE)2;
  g_mock_close_ok = 1;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID,
                                                -1, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_CPUSET, 0,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_JAIL, 42,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_DOMAIN, 42,
                                      sizeof(mask), &mask));

  /* Process CloseHandle failure on success path */
  g_mock_close_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_close_ok = 1;

  /* Process GetProcessAffinityMask failure for current (id=0) & external (42)
   */
  g_mock_get_proc_aff_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                          sizeof(mask), &mask));
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_get_proc_aff_ok = 1;

  /* OpenProcess NULL */
  g_mock_open_proc_ret = NULL;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_open_proc_ret = (HANDLE)2;

  /* Thread which variants: TID, ITHREAD, IRQ, INTRHANDLER; id = -1, 0, >0 */
  g_mock_set_thread_aff_ret = 1;
  g_mock_open_thread_ret = (HANDLE)3;
  g_mock_close_ok = 1;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID,
                                                -1, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_ITHREAD, 0,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_IRQ,
                                                55, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_INTRHANDLER,
                                      55, sizeof(mask), &mask));

  /* Thread CloseHandle failure on success path */
  g_mock_close_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_close_ok = 1;

  /* Thread SetThreadAffinityMask failure for current (0) & external (55) */
  g_mock_set_thread_aff_ret = 0;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0,
                                          sizeof(mask), &mask));
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_set_thread_aff_ret = 1;

  /* OpenThread NULL */
  g_mock_open_thread_ret = NULL;
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_open_thread_ret = (HANDLE)3;

  /* Unknown which & unknown level */
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, 999, 0, sizeof(mask),
                                          &mask));
  ASSERT_EQ(-1, mocked_cpuset_getaffinity(999, CPU_WHICH_PID, 0, sizeof(mask),
                                          &mask));
#if defined(_WIN64)
  g_mock_set_thread_aff_ret = ((DWORD_PTR)1) << 33;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID,
                                                0, sizeof(mask), &mask));
  ASSERT_EQ(0, CPU_ISSET(0, &mask));
  ASSERT_EQ(1, CPU_ISSET(33, &mask));
  g_mock_set_thread_aff_ret = 1;
#endif
#endif

  PASS();
}

TEST test_cpuset_setaffinity(void) {
  cpuset_t mask;

  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  ASSERT_EQ(-1, cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                   sizeof(mask), NULL));
  ASSERT_EQ(-1,
            cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, 0, &mask));
  (void)cpuset_setaffinity(CPU_LEVEL_CPUSET, (cpuwhich_t)-1, 0, sizeof(mask),
                           &mask);
  (void)cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, (id_t)-2,
                           sizeof(mask), &mask);

  CPU_ZERO(&mask);
  ASSERT_EQ(-1, cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                   sizeof(mask), &mask));

  CPU_SET(0, &mask);
  ASSERT_EQ(-1, cpuset_setaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0,
                                   sizeof(mask), &mask));
  ASSERT_EQ(-1, cpuset_setaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask));
  (void)cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  (void)cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);

#if defined(_MSC_VER) || defined(__MINGW32__)
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                          sizeof(mask), NULL));
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, 0,
                                          &mask));

  CPU_ZERO(&mask);
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                          sizeof(mask), &mask));
  CPU_SET(0, &mask);

  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0,
                                          sizeof(mask), &mask));

  /* Process variants */
  g_mock_set_proc_aff_ok = 1;
  g_mock_open_proc_ret = (HANDLE)2;
  g_mock_close_ok = 1;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID,
                                                -1, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_CPUSET, 0,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_JAIL, 42,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_DOMAIN, 42,
                                      sizeof(mask), &mask));

  g_mock_close_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_close_ok = 1;

  g_mock_set_proc_aff_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0,
                                          sizeof(mask), &mask));
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_set_proc_aff_ok = 1;

  g_mock_open_proc_ret = NULL;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 42,
                                          sizeof(mask), &mask));
  g_mock_open_proc_ret = (HANDLE)2;

  /* Thread variants */
  g_mock_set_thread_aff_ret = 1;
  g_mock_open_thread_ret = (HANDLE)3;
  g_mock_close_ok = 1;
  ASSERT_EQ(ERR_NONE, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID,
                                                -1, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_ITHREAD, 0,
                                      sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_IRQ,
                                                55, sizeof(mask), &mask));
  ASSERT_EQ(ERR_NONE,
            mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_INTRHANDLER,
                                      55, sizeof(mask), &mask));

  g_mock_close_ok = 0;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_close_ok = 1;

  g_mock_set_thread_aff_ret = 0;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0,
                                          sizeof(mask), &mask));
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_set_thread_aff_ret = 1;

  g_mock_open_thread_ret = NULL;
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 55,
                                          sizeof(mask), &mask));
  g_mock_open_thread_ret = (HANDLE)3;

  /* Unknown which & unknown level */
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(CPU_LEVEL_WHICH, 999, 0, sizeof(mask),
                                          &mask));
  ASSERT_EQ(-1, mocked_cpuset_setaffinity(999, CPU_WHICH_PID, 0, sizeof(mask),
                                          &mask));
#if defined(_WIN64)
  CPU_ZERO(&mask);
  CPU_SET(33, &mask);
  ASSERT_EQ(ERR_NONE, mocked_cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID,
                                                0, sizeof(mask), &mask));
#endif
#endif

  PASS();
}

SUITE(suite_bsd_sys_cpuset_core) {
  RUN_TEST(test_bsd_sys_cpuset_init);
  RUN_TEST(test_cpuset_macros_and_getid);
  RUN_TEST(test_cpuset_getaffinity);
  RUN_TEST(test_cpuset_setaffinity);
}

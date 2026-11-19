#if !defined(_WIN32) && !defined(_MSC_VER)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE 1
#endif
#endif

/* clang-format off */
#include "greatest.h"
#include "posix-wait.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32) && !defined(_MSC_VER)
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);

extern void posix_wait_set_open_wait_hooks(
    void *(*open_fn)(unsigned long, int, unsigned long),
    unsigned long (*single_fn)(void *, unsigned long));
extern void
posix_wait_set_proc_hooks(int (*exit_code_fn)(void *, unsigned long *),
                          int (*terminate_fn)(void *, unsigned int),
                          int (*close_fn)(void *), int (*reap_fn)(int, int *));
extern void posix_wait_reset_hooks(void);
extern void posix_wait_test_set_children(int count, void *h0,
                                         unsigned long pid0);

static void *g_mock_open_ret = NULL;
static unsigned long g_mock_wait_ret = 0UL;
static int g_mock_wait_step = 0;
static int g_mock_wait_delayed = 0;
static int g_mock_exit_ok = 1;
static unsigned long g_mock_exit_val = 7UL;
static int g_mock_term_ok = 1;
static int g_mock_reap_ret = 0;

static void *mock_open(unsigned long access, int inherit, unsigned long pid) {
  (void)access;
  (void)inherit;
  (void)pid;
  return g_mock_open_ret;
}

static unsigned long mock_single(void *h, unsigned long ms) {
  (void)h;
  (void)ms;
  if (g_mock_wait_delayed) {
    g_mock_wait_step++;
    return (g_mock_wait_step == 1) ? 258UL /* WAIT_TIMEOUT */ : 0UL;
  }
  return g_mock_wait_ret;
}

static int mock_exit_code(void *h, unsigned long *code) {
  (void)h;
  *code = g_mock_exit_val;
  return g_mock_exit_ok;
}

static int mock_terminate(void *h, unsigned int sig) {
  (void)h;
  (void)sig;
  return g_mock_term_ok;
}

static int mock_close(void *h) {
  (void)h;
  return 1;
}

static int mock_reap(int pid, int *stat_loc) {
  (void)pid;
  if (g_mock_reap_ret > 0 && stat_loc != NULL) {
    *stat_loc = (3 << 8);
  }
  return g_mock_reap_ret;
}
#endif

TEST test_posix_wait_get_info(void) {
  int avail = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_wait_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_wait_get_info(&avail));
  ASSERT_EQ(1, avail);
  PASS();
}

TEST test_wait_macros(void) {
  int status_exited = (42 << 8);
  int status_signaled = 9;
  int status_stopped = (17 << 8) | 0x7f;

  ASSERT(WIFEXITED(status_exited));
  ASSERT_EQ(42, WEXITSTATUS(status_exited));
  ASSERT(!WIFSIGNALED(status_exited));

  ASSERT(WIFSIGNALED(status_signaled));
  ASSERT_EQ(9, WTERMSIG(status_signaled));
  ASSERT(!WIFEXITED(status_signaled));

  ASSERT(WIFSTOPPED(status_stopped));
  ASSERT_EQ(17, WSTOPSIG(status_stopped));

  PASS();
}

TEST test_wait_basic_calls(void) {
  int status = 0;
  siginfo_t info;
  long rusage_buf[64];

  ASSERT_EQ((pid_t)-1, wait(&status));
  ASSERT_EQ((pid_t)-1, waitpid(-1, &status, WNOHANG));
  memset(rusage_buf, 0xAA, sizeof(rusage_buf));
  ASSERT_EQ((pid_t)-1,
            wait3(&status, WNOHANG, (struct rusage *)(void *)rusage_buf));
  ASSERT_EQ((pid_t)-1, wait3(&status, WNOHANG, NULL));
  ASSERT_EQ((intptr_t)-1, posix_cwait(&status, -1, 0));

  memset(&info, 0, sizeof(info));
  ASSERT_EQ(-1, waitid(P_ALL, 0, &info, WEXITED | WNOHANG));
  ASSERT_EQ(-1, waitid((idtype_t)999, 0, NULL, WEXITED));

#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  {
    pid_t cur_pid = (pid_t)GetCurrentProcessId();
    ASSERT_EQ(0, waitpid(cur_pid, &status, WNOHANG));
    ASSERT_EQ(0, posix_wait_kill(cur_pid, 0));
    ASSERT_EQ(-1, posix_wait_kill(0, 0));
    ASSERT_EQ(-1, posix_wait_kill(99999999, 0));
    ASSERT_EQ(-1, posix_wait_kill(99999999, 9));
  }
#else
  ASSERT_EQ(0, posix_wait_kill(getpid(), 0));
#endif

  PASS();
}

#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
TEST test_wait_win32_branches(void) {
  int status = 0;
  siginfo_t info;

  /* Invalid pid <= 0 && != -1 */
  ASSERT_EQ((pid_t)-1, waitpid(-2, &status, 0));
  ASSERT_EQ(EINVAL, errno);

  /* Exercise default_wait_exit_code and default_wait_terminate on harmless
   * invalid handle (-2) before overriding proc hooks */
  posix_wait_reset_hooks();
  g_mock_open_ret = (void *)(ptrdiff_t)(-2);
  g_mock_wait_ret = 0UL; /* WAIT_OBJECT_0 */
  g_mock_wait_delayed = 0;
  posix_wait_set_open_wait_hooks(mock_open, mock_single);
  ASSERT_EQ((pid_t)1234, waitpid(1234, &status, 0));
  ASSERT_EQ(0, status);
  ASSERT_EQ(-1, posix_wait_kill(1234, 9));

  /* Now install full hooks */
  posix_wait_set_proc_hooks(mock_exit_code, mock_terminate, mock_close,
                            mock_reap);

  /* 1. awm_vfork_try_reap returning > 0 */
  g_mock_reap_ret = 777;
  ASSERT_EQ((pid_t)777, waitpid(-1, &status, 0));
  ASSERT_EQ(3, WEXITSTATUS(status));
  g_mock_reap_ret = 0;

  /* 2. waitpid(-1, ...): child table with WNOHANG timeout, delayed Sleep(10)
   * wakeup, stat_loc != NULL (exit ok & fail), and stat_loc == NULL */
  posix_wait_test_set_children(1, (void *)(size_t)0x10, 501UL);
  g_mock_wait_ret = 258UL; /* WAIT_TIMEOUT */
  ASSERT_EQ((pid_t)0, waitpid(-1, &status, WNOHANG));

  /* Delayed wakeup: 1st check WAIT_TIMEOUT -> Sleep(10) -> 2nd check
   * WAIT_OBJECT_0 */
  g_mock_wait_delayed = 1;
  g_mock_wait_step = 0;
  g_mock_exit_ok = 1;
  g_mock_exit_val = 42UL;
  ASSERT_EQ((pid_t)501, waitpid(-1, &status, 0));
  ASSERT_EQ(42, WEXITSTATUS(status));
  g_mock_wait_delayed = 0;

  /* stat_loc != NULL with GetExitCodeProcess failing */
  posix_wait_test_set_children(1, (void *)(size_t)0x11, 502UL);
  g_mock_wait_ret = 0UL;
  g_mock_exit_ok = 0;
  status = 999;
  ASSERT_EQ((pid_t)502, waitpid(-1, &status, 0));
  ASSERT_EQ(0, status);

  /* stat_loc == NULL */
  posix_wait_test_set_children(1, (void *)(size_t)0x12, 503UL);
  ASSERT_EQ((pid_t)503, waitpid(-1, NULL, 0));

  /* 3. waitpid(pid > 0): in child table (found_idx != -1) vs non-matching child
   * entry + OpenProcess (found_idx == -1) */
  posix_wait_test_set_children(1, (void *)(size_t)0x20, 600UL);

  /* Non-matching pid 601 while table has 600 -> OpenProcess NULL -> ECHILD */
  g_mock_open_ret = NULL;
  ASSERT_EQ((pid_t)-1, waitpid(601, &status, 0));

  /* Matching pid 600 with WAIT_TIMEOUT */
  g_mock_wait_ret = 258UL;
  ASSERT_EQ((pid_t)0, waitpid(600, &status, WNOHANG));

  /* Matching pid 600 with WAIT_FAILED */
  g_mock_wait_ret = 0xFFFFFFFFUL;
  ASSERT_EQ((pid_t)-1, waitpid(600, &status, 0));

  /* Matching pid 600 with WAIT_OBJECT_0 and stat_loc != NULL (exit ok) */
  g_mock_wait_ret = 0UL;
  g_mock_exit_ok = 1;
  g_mock_exit_val = 9UL;
  ASSERT_EQ((pid_t)600, waitpid(600, &status, 0));
  ASSERT_EQ(9, WEXITSTATUS(status));

  /* Matching pid 602 with stat_loc == NULL */
  posix_wait_test_set_children(1, (void *)(size_t)0x21, 602UL);
  ASSERT_EQ((pid_t)602, waitpid(602, NULL, 0));

  /* Non-table pid (found_idx == -1) with WAIT_FAILED */
  g_mock_open_ret = (void *)(size_t)0x30;
  g_mock_wait_ret = 0xFFFFFFFFUL;
  ASSERT_EQ((pid_t)-1, waitpid(700, &status, 0));

  /* Non-table pid with WAIT_OBJECT_0 and GetExitCodeProcess ok */
  g_mock_wait_ret = 0UL;
  g_mock_exit_ok = 1;
  g_mock_exit_val = 5UL;
  ASSERT_EQ((pid_t)700, waitpid(700, &status, 0));
  ASSERT_EQ(5, WEXITSTATUS(status));

  /* 4. waitid: P_PID & P_ALL with res > 0 (infop != NULL & NULL) and res == 0
   */
  memset(&info, 0, sizeof(info));
  ASSERT_EQ(0, waitid(P_PID, 700, &info, WEXITED));
  ASSERT_EQ((pid_t)700, info.si_pid);
  ASSERT_EQ(5, info.si_status);
  ASSERT_EQ(0, waitid(P_PID, 700, NULL, WEXITED));

  g_mock_wait_ret = 258UL;
  ASSERT_EQ(0, waitid(P_PID, 700, &info, WEXITED | WNOHANG));

  /* 5. posix_wait_kill: TerminateProcess success */
  g_mock_open_ret = (void *)(size_t)0x40;
  g_mock_term_ok = 1;
  ASSERT_EQ(0, posix_wait_kill(700, 9));

  posix_wait_test_set_children(0, NULL, 0UL);
  posix_wait_reset_hooks();
  PASS();
}
#endif

SUITE(suite_posix_wait_core) {
  RUN_TEST(test_posix_wait_get_info);
  RUN_TEST(test_wait_macros);
  RUN_TEST(test_wait_basic_calls);
#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  RUN_TEST(test_wait_win32_branches);
#endif
}

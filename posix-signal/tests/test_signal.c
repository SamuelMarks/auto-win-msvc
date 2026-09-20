#ifdef _MSC_VER
#endif /* _MSC_VER */

/* clang-format off */
#include "greatest.h"
#include "posix-signal.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

static int g_test_handler_called = 0;
static void test_custom_handler(int signum) {
  (void)signum;
  g_test_handler_called++;
}

#if defined(_WIN32) || defined(_MSC_VER)
static int g_test_sigaction_called = 0;
static void test_custom_sigaction(int signum, siginfo_t *si, void *ctx) {
  (void)signum;
  (void)si;
  (void)ctx;
  g_test_sigaction_called++;
}

typedef struct _STARTUPINFOA {
  unsigned long cb;
  char *lpReserved;
  char *lpDesktop;
  char *lpTitle;
  unsigned long dwX;
  unsigned long dwY;
  unsigned long dwXSize;
  unsigned long dwYSize;
  unsigned long dwXCountChars;
  unsigned long dwYCountChars;
  unsigned long dwFillAttribute;
  unsigned long dwFlags;
  unsigned short wShowWindow;
  unsigned short cbReserved2;
  unsigned char *lpReserved2;
  void *hStdInput;
  void *hStdOutput;
  void *hStdError;
} STARTUPINFOA;

typedef struct _PROCESS_INFORMATION {
  void *hProcess;
  void *hThread;
  unsigned long dwProcessId;
  unsigned long dwThreadId;
} PROCESS_INFORMATION;

__declspec(dllimport) int __stdcall CreateProcessA(
    const char *lpApplicationName, char *lpCommandLine,
    void *lpProcessAttributes, void *lpThreadAttributes, int bInheritHandles,
    unsigned long dwCreationFlags, void *lpEnvironment,
    const char *lpCurrentDirectory, STARTUPINFOA *lpStartupInfo,
    PROCESS_INFORMATION *lpProcessInformation);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);
#endif

TEST test_posix_signal_get_info(void) {
  enum posix_signal_error_code rc;
  int avail;

  avail = 0;
  rc = posix_signal_get_info(NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_get_info(&avail);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, avail);

  PASS();
}

TEST test_posix_signal_init(void) {
  enum posix_signal_error_code rc;
  int status;

  status = 0;
  rc = posix_signal_init(NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_init(&status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, status);

  PASS();
}

TEST test_signal_constants(void) {
#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(1, SIGHUP);
  ASSERT_EQ(2, SIGINT);
  ASSERT_EQ(3, SIGQUIT);
  ASSERT_EQ(9, SIGKILL);
  ASSERT_EQ(10, SIGUSR1);
  ASSERT_EQ(12, SIGUSR2);
  ASSERT_EQ(13, SIGPIPE);
  ASSERT_EQ(14, SIGALRM);
  ASSERT_EQ(15, SIGTERM);
  ASSERT_EQ(17, SIGCHLD);
  ASSERT_EQ(1, SA_NOCLDSTOP);
  ASSERT_EQ(2, SA_NOCLDWAIT);
  ASSERT_EQ(4, SA_SIGINFO);
#else
  ASSERT(SIGINT > 0);
  ASSERT(SIGTERM > 0);
  ASSERT(SIGPIPE > 0);
#endif
  PASS();
}

TEST test_posix_signal_sigemptyset(void) {
  sigset_t set;
  int res;

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigemptyset(NULL);
  ASSERT_EQ(-1, res);
#endif

  memset(&set, 0x55, sizeof(set));
  res = sigemptyset(&set);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigfillset(void) {
  sigset_t set;
  int res;

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigfillset(NULL);
  ASSERT_EQ(-1, res);
#endif

  memset(&set, 0, sizeof(set));
  res = sigfillset(&set);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigaddset(void) {
  sigset_t set;
  int res;

  sigemptyset(&set);

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigaddset(NULL, SIGINT);
  ASSERT_EQ(-1, res);

  res = sigaddset(&set, 0);
  ASSERT_EQ(-1, res);

  res = sigaddset(&set, 32);
  ASSERT_EQ(-1, res);
#endif

  res = sigaddset(&set, SIGINT);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigdelset(void) {
  sigset_t set;
  int res;

  sigfillset(&set);

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigdelset(NULL, SIGINT);
  ASSERT_EQ(-1, res);

  res = sigdelset(&set, 0);
  ASSERT_EQ(-1, res);

  res = sigdelset(&set, 32);
  ASSERT_EQ(-1, res);
#endif

  res = sigdelset(&set, SIGINT);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigismember(void) {
  sigset_t set;
  int res;

  sigemptyset(&set);

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigismember(NULL, SIGINT);
  ASSERT_EQ(-1, res);

  res = sigismember(&set, 0);
  ASSERT_EQ(-1, res);

  res = sigismember(&set, 32);
  ASSERT_EQ(-1, res);
#endif

  res = sigismember(&set, SIGINT);
  ASSERT_EQ(0, res);

  sigaddset(&set, SIGINT);
  res = sigismember(&set, SIGINT);
  ASSERT_EQ(1, res);

  PASS();
}

TEST test_posix_signal_sigprocmask(void) {
  sigset_t set;
  sigset_t oldset;
  int res;

  sigemptyset(&set);
  sigaddset(&set, SIGINT);

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigprocmask(9999, &set, NULL);
  ASSERT_EQ(-1, res);
#endif

  res = sigprocmask(SIG_BLOCK, &set, &oldset);
  ASSERT_EQ(0, res);

  res = sigprocmask(SIG_UNBLOCK, &set, NULL);
  ASSERT_EQ(0, res);

  res = sigprocmask(SIG_SETMASK, &oldset, NULL);
  ASSERT_EQ(0, res);

  res = sigprocmask(SIG_SETMASK, NULL, &oldset);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigpending(void) {
  sigset_t set;
  int res;

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigpending(NULL);
  ASSERT_EQ(-1, res);
#endif

  res = sigpending(&set);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_posix_signal_sigsuspend(void) {
  sigset_t mask;
  int res;

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigsuspend(NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  sigemptyset(&mask);
  res = sigsuspend(&mask);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINTR, errno);
#else
  (void)mask;
  (void)res;
#endif

  PASS();
}

TEST test_posix_signal_sigaction(void) {
  struct sigaction sa;
  struct sigaction oldsa;
  int res;

#if defined(_WIN32) || defined(_MSC_VER)
  res = sigaction(0, &sa, NULL);
  ASSERT_EQ(-1, res);

  res = sigaction(32, &sa, NULL);
  ASSERT_EQ(-1, res);

  res = sigaction(SIGINT, NULL, NULL);
  ASSERT_EQ(-1, res);

  /* Test non-CRT signal handling */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = test_custom_handler;
  sa.sa_flags = 0;
  res = sigaction(SIGUSR1, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Set non-CRT signal without querying oldact */
  res = sigaction(SIGUSR1, &sa, NULL);
  ASSERT_EQ(0, res);

  /* Query oldact for non-CRT signal */
  memset(&oldsa, 0, sizeof(oldsa));
  res = sigaction(SIGUSR1, NULL, &oldsa);
  ASSERT_EQ(0, res);
  ASSERT_EQ(test_custom_handler, oldsa.sa_handler);

  /* SA_SIGINFO for non-CRT signal */
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = test_custom_sigaction;
  sa.sa_flags = SA_SIGINFO;
  res = sigaction(SIGUSR1, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Query oldact again */
  res = sigaction(SIGUSR1, NULL, &oldsa);
  ASSERT_EQ(0, res);
  ASSERT_EQ(test_custom_sigaction, oldsa.sa_sigaction);

  /* Test CRT signal with SA_SIGINFO */
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = test_custom_sigaction;
  sa.sa_flags = SA_SIGINFO;
  res = sigaction(SIGINT, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Query oldact for CRT signal */
  res = sigaction(SIGINT, NULL, &oldsa);
  ASSERT_EQ(0, res);

  /* Test CRT signal with custom handler */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = test_custom_handler;
  sa.sa_flags = 0;
  res = sigaction(SIGINT, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Test CRT signal with SIG_DFL */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_DFL;
  res = sigaction(SIGINT, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Test CRT signal with SIG_IGN */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  res = sigaction(SIGINT, &sa, &oldsa);
  ASSERT_EQ(0, res);

  /* Restore SIG_DFL */
  sa.sa_handler = SIG_DFL;
  sigaction(SIGINT, &sa, NULL);
#else
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = test_custom_handler;
  sigemptyset(&sa.sa_mask);
  res = sigaction(SIGPIPE, &sa, &oldsa);
  ASSERT_EQ(0, res);
  sigaction(SIGPIPE, &oldsa, NULL);
#endif

  PASS();
}

TEST test_posix_signal_signal(void) {
#if defined(_WIN32) || defined(_MSC_VER)
  posix_sighandler_t prev;

  prev = signal(0, test_custom_handler);
  ASSERT_EQ(SIG_ERR, prev);

  prev = signal(32, test_custom_handler);
  ASSERT_EQ(SIG_ERR, prev);

  /* Non-CRT signal */
  prev = signal(SIGUSR1, test_custom_handler);
  ASSERT(prev != SIG_ERR);

  prev = signal(SIGUSR1, SIG_DFL);
  ASSERT_EQ(test_custom_handler, prev);

  /* CRT signal */
  prev = signal(SIGINT, test_custom_handler);
  ASSERT(prev != SIG_ERR);

  prev = signal(SIGINT, SIG_DFL);
  ASSERT(prev != SIG_ERR);
#else
  void (*prev)(int);
  prev = signal(SIGPIPE, SIG_IGN);
  ASSERT(prev != SIG_ERR);
  signal(SIGPIPE, prev);
#endif

  PASS();
}

TEST test_posix_signal_kill(void) {
  int res;
#if defined(_WIN32) || defined(_MSC_VER)
  pid_t cur_pid;
  enum posix_signal_error_code rc;

  cur_pid = 0;
  rc = posix_signal_get_current_pid(NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_get_current_pid(&cur_pid);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT(cur_pid > 0);

  res = kill(0, 0);
  ASSERT_EQ(-1, res);

  res = kill(-1, 0);
  ASSERT_EQ(-1, res);

  res = kill(9999999, 0);
  ASSERT_EQ(-1, res);

  res = kill(9999999, SIGTERM);
  ASSERT_EQ(-1, res);

  res = kill(cur_pid, 0);
  ASSERT_EQ(0, res);

  {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char cmd[128];
    int created;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));
#if defined(_MSC_VER)
    strcpy_s(cmd, sizeof(cmd), "cmd.exe /c pause");
#else
    strcpy(cmd, "cmd.exe /c pause");
#endif

    created = CreateProcessA(NULL, cmd, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
    if (created) {
      res = kill((pid_t)pi.dwProcessId, SIGTERM);
      ASSERT_EQ(0, res);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    }
  }
#else
  res = kill(0, 0);
  (void)res;
#endif

  PASS();
}

#if defined(_WIN32) || defined(_MSC_VER)
TEST test_posix_signal_is_crt_signal(void) {
  enum posix_signal_error_code rc;
  int is_crt;

  is_crt = 0;
  rc = posix_signal_is_crt_signal(SIGINT, NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_is_crt_signal(SIGINT, &is_crt);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, is_crt);

  rc = posix_signal_is_crt_signal(SIGSEGV, &is_crt);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, is_crt);

  rc = posix_signal_is_crt_signal(SIGUSR1, &is_crt);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(0, is_crt);

  PASS();
}

TEST test_posix_signal_simulations(void) {
  enum posix_signal_error_code rc;
  int status;
  long result;
  sigset_t set;
  struct sigaction sa;

  /* Reset test */
  rc = posix_signal_reset(NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_reset(&status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, status);

  /* Simulate signal test */
  rc = posix_signal_simulate_signal(SIGINT, NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  rc = posix_signal_simulate_signal(0, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);

  rc = posix_signal_simulate_signal(32, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);

  /* Signal blocked in simulation */
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_SETMASK, &set, NULL);

  rc = posix_signal_simulate_signal(SIGUSR1, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);

  /* Verify it became pending */
  sigemptyset(&set);
  sigpending(&set);
  ASSERT_EQ(1, sigismember(&set, SIGUSR1));

  /* Setup handler and unblock to test delivery of pending signal */
  signal(SIGUSR1, test_custom_handler);
  g_test_handler_called = 0;
  sigemptyset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL);
  ASSERT_EQ(1, g_test_handler_called);

  /* Setup SA_SIGINFO handler on blocked signal */
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = test_custom_sigaction;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR2, &sa, NULL);

  sigemptyset(&set);
  sigaddset(&set, SIGUSR2);
  sigprocmask(SIG_SETMASK, &set, NULL);

  rc = posix_signal_simulate_signal(SIGUSR2, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);

  g_test_sigaction_called = 0;
  sigemptyset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL);
  ASSERT_EQ(1, g_test_sigaction_called);

  /* Direct simulation unblocked with SA_SIGINFO */
  g_test_sigaction_called = 0;
  rc = posix_signal_simulate_signal(SIGUSR2, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_sigaction_called);

  /* Direct simulation unblocked with handler */
  g_test_handler_called = 0;
  rc = posix_signal_simulate_signal(SIGUSR1, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_handler_called);

  /* Direct simulation with default/ignore handler */
  signal(SIGUSR1, SIG_DFL);
  rc = posix_signal_simulate_signal(SIGUSR1, &status);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);

  /* Test VEH simulations */
  rc = posix_signal_simulate_exception(0, NULL);
  ASSERT_EQ(POSIX_SIGNAL_ERROR_NULL_POINTER, rc);

  /* Null ExceptionInfo */
  rc = posix_signal_simulate_exception(0, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(0, result);

  /* Null ExceptionRecord */
  rc = posix_signal_simulate_exception(1, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(0, result);

  /* Unknown code */
  rc = posix_signal_simulate_exception(0x12345678UL, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(0, result); /* EXCEPTION_CONTINUE_SEARCH */

  /* Block SIGSEGV and simulate ACCESS_VIOLATION */
  sigemptyset(&set);
  sigaddset(&set, SIGSEGV);
  sigprocmask(SIG_SETMASK, &set, NULL);
  rc = posix_signal_simulate_exception(0xC0000005UL, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(0, result);

  /* Unblock SIGSEGV */
  sigemptyset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL);

  /* Setup SA_SIGINFO on SIGSEGV */
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = test_custom_sigaction;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);

  g_test_sigaction_called = 0;
  rc = posix_signal_simulate_exception(0xC0000005UL, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_sigaction_called);

  /* Setup regular handler on SIGILL */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = test_custom_handler;
  sa.sa_flags = 0;
  sigaction(SIGILL, &sa, NULL);

  g_test_handler_called = 0;
  rc = posix_signal_simulate_exception(0xC000001DUL, &result);
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_handler_called);

  /* Test SIGFPE exceptions */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = test_custom_handler;
  sa.sa_flags = 0;
  sigaction(SIGFPE, &sa, NULL);

  g_test_handler_called = 0;
  rc = posix_signal_simulate_exception(0xC0000094UL,
                                       &result); /* INT_DIVIDE_BY_ZERO */
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_handler_called);

  g_test_handler_called = 0;
  rc = posix_signal_simulate_exception(0xC000008EUL,
                                       &result); /* FLT_DIVIDE_BY_ZERO */
  ASSERT_EQ(POSIX_SIGNAL_SUCCESS, rc);
  ASSERT_EQ(1, g_test_handler_called);

  /* Cleanup */
  posix_signal_reset(&status);

  PASS();
}
#endif

SUITE(suite_posix_signal_signal) {
  RUN_TEST(test_posix_signal_get_info);
  RUN_TEST(test_posix_signal_init);
  RUN_TEST(test_signal_constants);
  RUN_TEST(test_posix_signal_sigemptyset);
  RUN_TEST(test_posix_signal_sigfillset);
  RUN_TEST(test_posix_signal_sigaddset);
  RUN_TEST(test_posix_signal_sigdelset);
  RUN_TEST(test_posix_signal_sigismember);
  RUN_TEST(test_posix_signal_sigprocmask);
  RUN_TEST(test_posix_signal_sigpending);
  RUN_TEST(test_posix_signal_sigsuspend);
  RUN_TEST(test_posix_signal_sigaction);
  RUN_TEST(test_posix_signal_signal);
  RUN_TEST(test_posix_signal_kill);
#if defined(_WIN32) || defined(_MSC_VER)
  RUN_TEST(test_posix_signal_is_crt_signal);
  RUN_TEST(test_posix_signal_simulations);
#endif
}

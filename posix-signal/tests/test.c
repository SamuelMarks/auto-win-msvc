#if !defined(_MSC_VER)
#define _POSIX_C_SOURCE 200809L
#endif
/* clang-format off */
#include "greatest.h"
#include "posix-signal.h"
#include <errno.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);
#else
#include <unistd.h>
#endif

static int sig_handled = 0;
static void my_handler(int sig) { sig_handled = sig; }

TEST test_sigset(void) {
  sigset_t set;
  ASSERT_EQ(0, sigemptyset(&set));
  ASSERT_EQ(0, sigaddset(&set, 2)); /* SIGINT is usually 2 */

  ASSERT_EQ(0, sigfillset(&set));

#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sigemptyset(NULL));
  ASSERT_EQ(-1, sigaddset(NULL, 2));
  ASSERT_EQ(-1, sigfillset(NULL));
  ASSERT_EQ(-1, sigaddset(&set, 0));
  ASSERT_EQ(-1, sigaddset(&set, 32));

  ASSERT_EQ(1, sigismember(&set, 2));
  ASSERT_EQ(0, sigdelset(&set, 2));
  ASSERT_EQ(0, sigismember(&set, 2));
#endif
  PASS();
}

TEST test_sigprocmask_basic(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sigset_t set, oldset, pendingset;
  struct sigaction act;

  sig_handled = 0;
  act.sa_handler = my_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGINT, &act, NULL);

  sigemptyset(&set);
  sigaddset(&set, SIGINT);

  /* Block SIGINT */
  ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &set, &oldset));

  /* Raise SIGINT directly */
  raise(SIGINT);

  /* It should be pending */
  ASSERT_EQ(0, sigpending(&pendingset));
  ASSERT_EQ(1, sigismember(&pendingset, SIGINT));

  /* Unblock SIGINT, it should trigger the handler and clear from pending */
  ASSERT_EQ(0, sigprocmask(SIG_UNBLOCK, &set, NULL));
  ASSERT_EQ(SIGINT, sig_handled);

  /* Should no longer be pending */
  ASSERT_EQ(0, sigpending(&pendingset));
  ASSERT_EQ(0, sigismember(&pendingset, SIGINT));

  /* Restore */
  act.sa_handler = SIG_DFL;
  sigaction(SIGINT, &act, NULL);
#endif
  PASS();
}

TEST test_sigaction_basic(void) {
  struct sigaction act, oldact;

#if defined(_WIN32) && !defined(__CYGWIN__)
  /* Invalid arguments */
  ASSERT_EQ(-1, sigaction(SIGINT, NULL, NULL));
  ASSERT_EQ(EINVAL, errno);
#endif

  /* Test setting handler */
  act.sa_handler = my_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  ASSERT_EQ(0, sigaction(SIGINT, &act, &oldact));

  /* Test just getting handler */
  ASSERT_EQ(0, sigaction(SIGINT, NULL, &oldact));
  ASSERT(my_handler == oldact.sa_handler);

#if defined(_WIN32) && !defined(__CYGWIN__)
  /* Test invalid signal */
  ASSERT_EQ(-1, sigaction(999, &act, NULL));
  ASSERT_EQ(EINVAL, errno);
#endif

  /* Restore SIG_DFL */
  act.sa_handler = SIG_DFL;
  ASSERT_EQ(0, sigaction(SIGINT, &act, NULL));

  PASS();
}

TEST test_kill(void) {
  pid_t my_pid;
#if defined(_WIN32) && !defined(__CYGWIN__)
  my_pid = (pid_t)GetCurrentProcessId();
#else
  my_pid = getpid();
#endif

  ASSERT_EQ(0, kill(my_pid, 0));

#if defined(_WIN32) && !defined(__CYGWIN__)
  /* Invalid pid */
  ASSERT_EQ(-1, kill(-1, 0));
#endif

  PASS();
}

SUITE(suite) {
  RUN_TEST(test_sigset);
  RUN_TEST(test_sigprocmask_basic);
  RUN_TEST(test_sigaction_basic);
  RUN_TEST(test_kill);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

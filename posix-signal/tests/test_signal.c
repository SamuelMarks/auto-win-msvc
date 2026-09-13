#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-signal.h"
#include <stddef.h>
/* clang-format on */

TEST test_signal(void) {
  if (sigaction(SIGINT, NULL, NULL) == -1)
    SKIP();
  PASS();
}

TEST test_signal_constants(void) {
#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(1, SIGHUP);
  ASSERT_EQ(2, SIGINT);
  ASSERT_EQ(3, SIGQUIT);
  ASSERT_EQ(10, SIGUSR1);
  ASSERT_EQ(12, SIGUSR2);
  ASSERT_EQ(13, SIGPIPE);
  ASSERT_EQ(14, SIGALRM);
  ASSERT_EQ(15, SIGTERM);
  ASSERT_EQ(17, SIGCHLD);
#else
  ASSERT(SIGINT > 0);
  ASSERT(SIGTERM > 0);
  ASSERT(SIGPIPE > 0);
#endif
  PASS();
}

TEST test_signal_sigpipe(void) {
#if defined(_WIN32) || defined(_MSC_VER)
  posix_sighandler_t prev;
#else
  void (*prev)(int);
#endif
  prev = signal(SIGPIPE, SIG_IGN);
  ASSERT(prev != SIG_ERR);
  PASS();
}

TEST test_sigaction_sigpipe(void) {
  struct sigaction sa;
  struct sigaction oldsa;
  int rc;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  rc = sigaction(SIGPIPE, &sa, &oldsa);
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_posix_signal_sigemptyset(void) {
  SKIP(); /* Generated stub for posix_signal_sigemptyset */
}

TEST test_posix_signal_sigfillset(void) {
  SKIP(); /* Generated stub for posix_signal_sigfillset */
}

TEST test_posix_signal_sigaddset(void) {
  SKIP(); /* Generated stub for posix_signal_sigaddset */
}

TEST test_posix_signal_sigaction(void) {
  SKIP(); /* Generated stub for posix_signal_sigaction */
}

TEST test_posix_signal_sigdelset(void) {
  SKIP(); /* Generated stub for posix_signal_sigdelset */
}

TEST test_posix_signal_sigismember(void) {
  SKIP(); /* Generated stub for posix_signal_sigismember */
}

TEST test_posix_signal_sigprocmask(void) {
  SKIP(); /* Generated stub for posix_signal_sigprocmask */
}

TEST test_posix_signal_sigpending(void) {
  SKIP(); /* Generated stub for posix_signal_sigpending */
}

TEST test_posix_signal_sigsuspend(void) {
  SKIP(); /* Generated stub for posix_signal_sigsuspend */
}

TEST test_posix_signal_kill(void) {
  SKIP(); /* Generated stub for posix_signal_kill */
}

SUITE(suite_posix_signal_signal) {
  RUN_TEST(test_signal);
  RUN_TEST(test_signal_constants);
  RUN_TEST(test_signal_sigpipe);
  RUN_TEST(test_sigaction_sigpipe);
  RUN_TEST(test_posix_signal_sigemptyset);
  RUN_TEST(test_posix_signal_sigfillset);
  RUN_TEST(test_posix_signal_sigaddset);
  RUN_TEST(test_posix_signal_sigaction);
  RUN_TEST(test_posix_signal_sigdelset);
  RUN_TEST(test_posix_signal_sigismember);
  RUN_TEST(test_posix_signal_sigprocmask);
  RUN_TEST(test_posix_signal_sigpending);
  RUN_TEST(test_posix_signal_sigsuspend);
  RUN_TEST(test_posix_signal_kill);
}

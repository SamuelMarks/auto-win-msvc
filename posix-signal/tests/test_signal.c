#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-signal.h"
#include <stddef.h>

TEST test_signal(void) {
    if (sigaction(SIGINT, NULL, NULL) == -1) SKIP();
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

/* test.c - 100% Test Coverage Stubs */
#include "greatest.h"
#include "posix-syslog.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* Test cases */

TEST test_openlog_closelog(void) {
    openlog("test_ident", LOG_PID | LOG_NDELAY, LOG_USER);
    closelog();
    
    openlog(NULL, LOG_PID | LOG_NDELAY, LOG_USER);
    closelog();
    
    PASS();
}

TEST test_setlogmask(void) {
    int old_mask = setlogmask(LOG_MASK(LOG_ERR));
    int new_mask = setlogmask(old_mask);
    ASSERT_EQ(LOG_MASK(LOG_ERR), new_mask);
    PASS();
}

TEST test_syslog_output(void) {
    openlog("test_syslog", LOG_PERROR, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));

    syslog(LOG_INFO, "This is an info message: %d", 42);
    syslog(LOG_ERR, "This is an error message: %s", "test");
    syslog(LOG_WARNING, "This is a warning message");
    syslog(LOG_EMERG, "This is an emergency");

    /* Test masking out messages */
    setlogmask(LOG_MASK(LOG_ERR));
    syslog(LOG_INFO, "This should not be logged (or trigger perror)");

    closelog();
    PASS();
}

TEST test_syslog_macros(void) {
    ASSERT_EQ(LOG_ERR, LOG_PRI(LOG_ERR | LOG_USER));
    ASSERT_EQ(LOG_USER >> 3, LOG_FAC(LOG_ERR | LOG_USER));
    ASSERT_EQ(LOG_ERR | LOG_USER, LOG_MAKEPRI(LOG_USER, LOG_ERR));
    ASSERT_EQ(1 << LOG_ERR, LOG_MASK(LOG_ERR));
    ASSERT_EQ((1 << (LOG_ERR + 1)) - 1, LOG_UPTO(LOG_ERR));
    PASS();
}

/* Add all test suites here */
SUITE(posix_syslog_suite) {
    RUN_TEST(test_openlog_closelog);
    RUN_TEST(test_setlogmask);
    RUN_TEST(test_syslog_output);
    RUN_TEST(test_syslog_macros);
}

/* Main test runner */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_syslog_suite);
    GREATEST_MAIN_END();
}

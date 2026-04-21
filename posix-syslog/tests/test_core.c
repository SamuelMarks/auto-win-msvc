#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-syslog.h"

TEST test_syslog(void) {
  openlog("test_syslog", LOG_PID | LOG_CONS, LOG_USER);
  syslog(LOG_INFO, "This is a test log message from posix-syslog.");
  closelog();
  PASS();
}

TEST test_closelog(void) { SKIP(); /* Generated stub for closelog */ }

TEST test_openlog(void) { SKIP(); /* Generated stub for openlog */ }

TEST test_setlogmask(void) { SKIP(); /* Generated stub for setlogmask */ }

SUITE(suite_posix_syslog_core) {
  RUN_TEST(test_syslog);
  RUN_TEST(test_closelog);
  RUN_TEST(test_openlog);
  RUN_TEST(test_setlogmask);
}

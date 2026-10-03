#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-syslog.h"
#include "syslog.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_syslog_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_syslog_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_syslog_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_syslog_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_syslog_lifecycle(void) {
  int old_mask;

  openlog("test_syslog", LOG_PID | LOG_PERROR | LOG_NDELAY, LOG_USER);
  syslog(LOG_INFO, "Info test: %s", "hello");
  syslog(LOG_EMERG, "Emerg test");
  syslog(LOG_WARNING, "Warning test");

  old_mask = setlogmask(LOG_MASK(LOG_ERR));
  ASSERT_NEQ(0, old_mask);

  /* Should be filtered out by mask */
  syslog(LOG_INFO, "Filtered info message");

  /* Mask 0 returns current without changing */
  ASSERT_EQ(LOG_MASK(LOG_ERR), setlogmask(0));

  /* Restore all mask */
  setlogmask(0xFF);
  closelog();

  /* Open with NULL ident */
  openlog(NULL, LOG_PERROR, LOG_DAEMON);
  syslog(LOG_ERR, "Error with NULL ident");
  closelog();

  PASS();
}

TEST test_syslog_macros(void) {
  ASSERT_EQ(LOG_INFO, LOG_PRI(LOG_INFO));
  ASSERT_EQ(LOG_USER >> 3, LOG_FAC(LOG_USER));
  ASSERT_EQ(LOG_USER | LOG_INFO, LOG_MAKEPRI(LOG_USER, LOG_INFO));
  ASSERT_EQ(1 << LOG_ERR, LOG_MASK(LOG_ERR));
  ASSERT_EQ((1 << (LOG_ERR + 1)) - 1, LOG_UPTO(LOG_ERR));
  PASS();
}

SUITE(suite_posix_syslog_core) {
  RUN_TEST(test_posix_syslog_get_info);
  RUN_TEST(test_syslog_lifecycle);
  RUN_TEST(test_syslog_macros);
}

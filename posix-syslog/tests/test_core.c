#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-syslog.h"
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

  /* Open without LOG_PERROR */
  openlog("no_perror", 0, LOG_USER);
  syslog(LOG_INFO, "Info without perror");

  /* Test truncation or error with very long string */
  {
    char long_str[5000];
    memset(long_str, 'A', sizeof(long_str) - 1);
    long_str[sizeof(long_str) - 1] = '\0';
    syslog(LOG_ERR, "Long error: %s", long_str);
  }

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

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif
#ifndef _MSC_VER
#define DEFINED_MSC_VER_FOR_TEST
#define _MSC_VER 1
#endif

#define MOCK_EVENTLOG 1

static int mock_register_fail = 0;

void *RegisterEventSourceA(const char *lpUNCServerName,
                           const char *lpSourceName) {
  (void)lpUNCServerName;
  (void)lpSourceName;
  if (mock_register_fail)
    return NULL;
  return (void *)1;
}
int DeregisterEventSource(void *hEventLog) {
  (void)hEventLog;
  return 1;
}
int ReportEventA(void *hEventLog, unsigned short wType,
                 unsigned short wCategory, unsigned long dwEventID,
                 void *lpUserSid, unsigned short wNumStrings,
                 unsigned long dwDataSize, const char **lpStrings,
                 void *lpRawData) {
  (void)hEventLog;
  (void)wType;
  (void)wCategory;
  (void)dwEventID;
  (void)lpUserSid;
  (void)wNumStrings;
  (void)dwDataSize;
  (void)lpStrings;
  (void)lpRawData;
  return 1;
}
#define _vsnprintf vsnprintf

static void *mock_malloc(size_t size) {
  (void)size;
  return NULL;
}
#include "../src/posix-syslog.c"

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif
#ifdef DEFINED_MSC_VER_FOR_TEST
#undef _MSC_VER
#endif

TEST test_syslog_oom(void) {
  posix_syslog_mock_malloc_ptr = mock_malloc;
  openlog("test_syslog_oom", LOG_PID, LOG_USER);
  /* g_Ident should be NULL due to OOM, but it shouldn't crash */
  syslog(LOG_INFO, "OOM test message");
  closelog();
  posix_syslog_mock_malloc_ptr = NULL;
  PASS();
}

TEST test_syslog_extra_branches(void) {
  /* Test openlog with NULL ident and NDELAY */
  openlog(NULL, LOG_NDELAY, LOG_USER);

  /* Test LOG_NOTICE and LOG_DEBUG */
  syslog(LOG_NOTICE, "notice");
  syslog(LOG_DEBUG, "debug");
  syslog(100, "default branch");
  closelog();

  /* Test RegisterEventSource failure */
  mock_register_fail = 1;
  openlog("fail", 0, LOG_USER);
  syslog(LOG_INFO, "should not report");
  closelog();
  mock_register_fail = 0;

  PASS();
}

SUITE(suite_posix_syslog_core) {
  RUN_TEST(test_posix_syslog_get_info);
  RUN_TEST(test_syslog_lifecycle);
  RUN_TEST(test_syslog_macros);
  RUN_TEST(test_syslog_oom);
  RUN_TEST(test_syslog_extra_branches);
}

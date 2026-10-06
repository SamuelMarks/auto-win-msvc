/* clang-format off */
#include "greatest.h"
#include <errno.h>
#include <stddef.h>

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif
#ifndef _MSC_VER
#define DEFINED_MSC_VER_FOR_TEST
#define _MSC_VER 1
#endif

#include "../src/sys_timerfd.c"

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif
#ifdef DEFINED_MSC_VER_FOR_TEST
#undef _MSC_VER
#endif
/* clang-format on */

TEST test_timerfd(void) {
  int rc;
  struct itimerspec ts;
  (void)rc;

  rc = timerfd_create(-1, 0);

  rc = timerfd_create(0, -1);

  rc = timerfd_create(0, 0);

  rc = timerfd_gettime(-1, &ts);

  rc = timerfd_gettime(0, NULL);

  rc = timerfd_gettime(0, &ts);

  rc = timerfd_settime(-1, 0, &ts, NULL);

  rc = timerfd_settime(0, -1, &ts, NULL);

  rc = timerfd_settime(0, 0, NULL, NULL);

  rc = timerfd_settime(0, 0, &ts, NULL);

  rc = timerfd_settime(0, 0, &ts, &ts);

  PASS();
}

SUITE(suite_sys_timerfd) { RUN_TEST(test_timerfd); }

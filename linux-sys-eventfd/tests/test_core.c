/* clang-format off */
#include "greatest.h"
#include <errno.h>

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif
#ifndef _MSC_VER
#define DEFINED_MSC_VER_FOR_TEST
#define _MSC_VER 1
#endif

#include "../src/sys_eventfd.c"

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif
#ifdef DEFINED_MSC_VER_FOR_TEST
#undef _MSC_VER
#endif
/* clang-format on */

TEST test_eventfd(void) {
  int rc;
  (void)rc;

  errno = 0;
  rc = eventfd(0, -1);

  errno = 0;
  rc = eventfd(0xFFFFFFFFU, 0);

  errno = 0;
  rc = eventfd(0, 0);

  PASS();
}

SUITE(suite_sys_eventfd) { RUN_TEST(test_eventfd); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_sys_eventfd);
  GREATEST_MAIN_END();
}

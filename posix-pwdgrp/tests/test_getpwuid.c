#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_getpwuid(void) { SKIP(); /* Generated stub for getpwuid */ }

TEST test_getpwuid_r(void) { SKIP(); /* Generated stub for getpwuid_r */ }

SUITE(suite_posix_pwdgrp_getpwuid) {
  RUN_TEST(test_getpwuid);
  RUN_TEST(test_getpwuid_r);
}

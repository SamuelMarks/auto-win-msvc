#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_getgrnam(void) { SKIP(); /* Generated stub for getgrnam */ }

TEST test_getgrnam_r(void) { SKIP(); /* Generated stub for getgrnam_r */ }

SUITE(suite_posix_pwdgrp_getgrnam) {
  RUN_TEST(test_getgrnam);
  RUN_TEST(test_getgrnam_r);
}

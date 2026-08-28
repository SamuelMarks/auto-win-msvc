#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_getpwnam(void) { SKIP(); /* Generated stub for getpwnam */ }

TEST test_getpwnam_r(void) { SKIP(); /* Generated stub for getpwnam_r */ }

SUITE(suite_posix_pwdgrp_getpwnam) {
  RUN_TEST(test_getpwnam);
  RUN_TEST(test_getpwnam_r);
}

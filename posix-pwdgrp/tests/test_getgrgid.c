#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_getgrgid(void) { SKIP(); /* Generated stub for getgrgid */ }

TEST test_getgrgid_r(void) { SKIP(); /* Generated stub for getgrgid_r */ }

SUITE(suite_posix_pwdgrp_getgrgid) {
  RUN_TEST(test_getgrgid);
  RUN_TEST(test_getgrgid_r);
}

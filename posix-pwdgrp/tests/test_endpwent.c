#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_endpwent(void) {
  struct passwd *pw;
  setpwent();
  pw = getpwent();
  ASSERT(pw != NULL || pw == NULL);
  endpwent();
  PASS();
}

SUITE(suite_posix_pwdgrp_endpwent) { RUN_TEST(test_endpwent); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>
/* clang-format on */

TEST test_pwdgrp(void) {
  struct passwd *pw;
  struct group *gr;

  setpwent();
  pw = getpwent();
  if (pw != NULL) {
    ASSERT(pw->pw_name != NULL);
    ASSERT(strlen(pw->pw_name) > 0);
  }
  endpwent();

  setgrent();
  gr = getgrent();
  if (gr != NULL) {
    ASSERT(gr->gr_name != NULL);
    ASSERT(strlen(gr->gr_name) > 0);
  }
  endgrent();

  PASS();
}

SUITE(suite_posix_pwdgrp_pwdgrp) { RUN_TEST(test_pwdgrp); }

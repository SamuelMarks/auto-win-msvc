#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>
/* clang-format on */

TEST test_getpwent(void) {
  struct passwd *pw;
  int count;

  setpwent();
  count = 0;
  while ((pw = getpwent()) != NULL && count < 5) {
    ASSERT(pw->pw_name != NULL);
    count++;
  }
  endpwent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getpwent) { RUN_TEST(test_getpwent); }

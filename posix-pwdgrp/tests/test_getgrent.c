#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>
/* clang-format on */

TEST test_getgrent(void) {
  struct group *gr;
  int count;

  setgrent();
  count = 0;
  while ((gr = getgrent()) != NULL && count < 5) {
    ASSERT(gr->gr_name != NULL);
    count++;
  }
  endgrent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getgrent) { RUN_TEST(test_getgrent); }

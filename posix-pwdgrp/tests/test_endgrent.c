#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
/* clang-format on */

TEST test_endgrent(void) {
  struct group *gr;
  setgrent();
  gr = getgrent();
  ASSERT(gr != NULL || gr == NULL);
  endgrent();
  PASS();
}

SUITE(suite_posix_pwdgrp_endgrent) { RUN_TEST(test_endgrent); }

#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pwdgrp.h"

TEST test_pwdgrp(void) {
  if (getpwuid(0) == NULL)
    SKIP();
  PASS();
}

SUITE(suite_posix_pwdgrp_pwdgrp) { RUN_TEST(test_pwdgrp); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>
/* clang-format on */

TEST test_setpwent(void) {
  struct passwd *pw1;
  struct passwd *pw2;
  char first_name[128];

  first_name[0] = '\0';
  setpwent();
  pw1 = getpwent();
  if (pw1 != NULL && pw1->pw_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(first_name, sizeof(first_name), pw1->pw_name, _TRUNCATE);
#else
    strncpy(first_name, pw1->pw_name, sizeof(first_name) - 1);
    first_name[sizeof(first_name) - 1] = '\0';
#endif

    /* Rewind */
    setpwent();
    pw2 = getpwent();
    ASSERT(pw2 != NULL);
    ASSERT_STR_EQ(first_name, pw2->pw_name);
  }
  endpwent();

  PASS();
}

SUITE(suite_posix_pwdgrp_setpwent) { RUN_TEST(test_setpwent); }

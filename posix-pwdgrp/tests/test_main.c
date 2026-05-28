#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_pwdgrp_pwdgrp);
SUITE_EXTERN(suite_posix_pwdgrp_endgrent);
SUITE_EXTERN(suite_posix_pwdgrp_getgrent);
SUITE_EXTERN(suite_posix_pwdgrp_getgrgid);
SUITE_EXTERN(suite_posix_pwdgrp_getgrnam);
SUITE_EXTERN(suite_posix_pwdgrp_setgrent);
SUITE_EXTERN(suite_posix_pwdgrp_endpwent);
SUITE_EXTERN(suite_posix_pwdgrp_getpwent);
SUITE_EXTERN(suite_posix_pwdgrp_getpwnam);
SUITE_EXTERN(suite_posix_pwdgrp_getpwuid);
SUITE_EXTERN(suite_posix_pwdgrp_setpwent);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_pwdgrp_pwdgrp);
  RUN_SUITE(suite_posix_pwdgrp_endgrent);
  RUN_SUITE(suite_posix_pwdgrp_getgrent);
  RUN_SUITE(suite_posix_pwdgrp_getgrgid);
  RUN_SUITE(suite_posix_pwdgrp_getgrnam);
  RUN_SUITE(suite_posix_pwdgrp_setgrent);
  RUN_SUITE(suite_posix_pwdgrp_endpwent);
  RUN_SUITE(suite_posix_pwdgrp_getpwent);
  RUN_SUITE(suite_posix_pwdgrp_getpwnam);
  RUN_SUITE(suite_posix_pwdgrp_getpwuid);
  RUN_SUITE(suite_posix_pwdgrp_setpwent);
  GREATEST_MAIN_END();
  return 0;
}

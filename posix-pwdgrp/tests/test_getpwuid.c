#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <errno.h>
/* clang-format on */

TEST test_getpwuid(void) {
  struct passwd *pw;
  struct passwd *found;

  setpwent();
  pw = getpwent();
  if (pw != NULL) {
    found = getpwuid(pw->pw_uid);
    if (found != NULL) {
      ASSERT_EQ(pw->pw_uid, found->pw_uid);
    }
  }
  endpwent();

  PASS();
}

TEST test_getpwuid_r(void) {
  struct passwd pwd;
  struct passwd *res;
  char buf[2048];
  struct passwd *pw;
  int rc;

#if defined(_WIN32)
  rc = getpwuid_r(0, NULL, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getpwuid_r(0, &pwd, NULL, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getpwuid_r(0, &pwd, buf, sizeof(buf), NULL);
  ASSERT_EQ(EINVAL, rc);
#endif

  res = (struct passwd *)1;
  rc = getpwuid_r((uid_t)99999999, &pwd, buf, sizeof(buf), &res);
  (void)rc;
  ASSERT_EQ(NULL, res);

  setpwent();
  pw = getpwent();
  if (pw != NULL) {
    rc = getpwuid_r(pw->pw_uid, &pwd, buf, 1, &res);
    ASSERT_EQ(ERANGE, rc);

    rc = getpwuid_r(pw->pw_uid, &pwd, buf, sizeof(buf), &res);
    if (rc == 0) {
      ASSERT_EQ(&pwd, res);
      ASSERT_EQ(pw->pw_uid, pwd.pw_uid);
    }
  }
  endpwent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getpwuid) {
  RUN_TEST(test_getpwuid);
  RUN_TEST(test_getpwuid_r);
}

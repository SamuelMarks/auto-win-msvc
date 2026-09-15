#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <errno.h>
#include <string.h>
/* clang-format on */

TEST test_getpwnam(void) {
  struct passwd *pw;
  struct passwd *found;
  char valid_name[128];

  valid_name[0] = '\0';
  ASSERT_EQ(NULL, getpwnam(NULL));
  ASSERT_EQ(NULL, getpwnam("non_existent_usr_1234567"));

  setpwent();
  pw = getpwent();
  if (pw != NULL && pw->pw_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(valid_name, sizeof(valid_name), pw->pw_name, _TRUNCATE);
#else
    strncpy(valid_name, pw->pw_name, sizeof(valid_name) - 1);
    valid_name[sizeof(valid_name) - 1] = '\0';
#endif
    found = getpwnam(valid_name);
    ASSERT(found != NULL);
    ASSERT_STR_EQ(valid_name, found->pw_name);
  }
  endpwent();

  PASS();
}

TEST test_getpwnam_r(void) {
  struct passwd pwd;
  struct passwd *res;
  char buf[2048];
  struct passwd *pw;
  int rc;
  char valid_name[128];

  valid_name[0] = '\0';
#if defined(_WIN32)
  rc = getpwnam_r(NULL, &pwd, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getpwnam_r("test", NULL, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getpwnam_r("test", &pwd, NULL, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getpwnam_r("test", &pwd, buf, sizeof(buf), NULL);
  ASSERT_EQ(EINVAL, rc);
#endif

  res = (struct passwd *)1;
  rc = getpwnam_r("non_existent_usr_1234567", &pwd, buf, sizeof(buf), &res);
  (void)rc;
  ASSERT_EQ(NULL, res);

  setpwent();
  pw = getpwent();
  if (pw != NULL && pw->pw_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(valid_name, sizeof(valid_name), pw->pw_name, _TRUNCATE);
#else
    strncpy(valid_name, pw->pw_name, sizeof(valid_name) - 1);
    valid_name[sizeof(valid_name) - 1] = '\0';
#endif

    rc = getpwnam_r(valid_name, &pwd, buf, 1, &res);
    ASSERT_EQ(ERANGE, rc);

    rc = getpwnam_r(valid_name, &pwd, buf, sizeof(buf), &res);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(&pwd, res);
    ASSERT_STR_EQ(valid_name, pwd.pw_name);
  }
  endpwent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getpwnam) {
  RUN_TEST(test_getpwnam);
  RUN_TEST(test_getpwnam_r);
}

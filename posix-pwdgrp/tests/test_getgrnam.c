#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <errno.h>
#include <string.h>
/* clang-format on */

TEST test_getgrnam(void) {
  struct group *gr;
  struct group *found;
  char valid_name[128];

  valid_name[0] = '\0';
  ASSERT_EQ(NULL, getgrnam(NULL));
  ASSERT_EQ(NULL, getgrnam("non_existent_grp_1234567"));

  setgrent();
  gr = getgrent();
  if (gr != NULL && gr->gr_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(valid_name, sizeof(valid_name), gr->gr_name, _TRUNCATE);
#else
    strncpy(valid_name, gr->gr_name, sizeof(valid_name) - 1);
    valid_name[sizeof(valid_name) - 1] = '\0';
#endif
    found = getgrnam(valid_name);
    ASSERT(found != NULL);
    ASSERT_STR_EQ(valid_name, found->gr_name);
  }
  endgrent();

  PASS();
}

TEST test_getgrnam_r(void) {
  struct group grp;
  struct group *res;
  char buf[2048];
  struct group *gr;
  int rc;
  char valid_name[128];

  valid_name[0] = '\0';
#if defined(_WIN32)
  rc = getgrnam_r(NULL, &grp, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getgrnam_r("test", NULL, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getgrnam_r("test", &grp, NULL, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getgrnam_r("test", &grp, buf, sizeof(buf), NULL);
  ASSERT_EQ(EINVAL, rc);
#endif

  res = (struct group *)1;
  rc = getgrnam_r("non_existent_grp_1234567", &grp, buf, sizeof(buf), &res);
  (void)rc;
  ASSERT_EQ(NULL, res);

  setgrent();
  gr = getgrent();
  if (gr != NULL && gr->gr_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(valid_name, sizeof(valid_name), gr->gr_name, _TRUNCATE);
#else
    strncpy(valid_name, gr->gr_name, sizeof(valid_name) - 1);
    valid_name[sizeof(valid_name) - 1] = '\0';
#endif

    rc = getgrnam_r(valid_name, &grp, buf, 1, &res);
    ASSERT_EQ(ERANGE, rc);

    rc = getgrnam_r(valid_name, &grp, buf, sizeof(buf), &res);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(&grp, res);
    ASSERT_STR_EQ(valid_name, grp.gr_name);
  }
  endgrent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getgrnam) {
  RUN_TEST(test_getgrnam);
  RUN_TEST(test_getgrnam_r);
}

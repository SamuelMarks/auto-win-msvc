#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <errno.h>
/* clang-format on */

TEST test_getgrgid(void) {
  struct group *gr;
  struct group *found;

  setgrent();
  gr = getgrent();
  if (gr != NULL) {
    found = getgrgid(gr->gr_gid);
    if (found != NULL) {
      ASSERT_EQ(gr->gr_gid, found->gr_gid);
    }
  }
  endgrent();

  PASS();
}

TEST test_getgrgid_r(void) {
  struct group grp;
  struct group *res;
  char buf[2048];
  struct group *gr;
  int rc;

#if defined(_WIN32)
  rc = getgrgid_r(0, NULL, buf, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getgrgid_r(0, &grp, NULL, sizeof(buf), &res);
  ASSERT_EQ(EINVAL, rc);

  rc = getgrgid_r(0, &grp, buf, sizeof(buf), NULL);
  ASSERT_EQ(EINVAL, rc);
#endif

  res = (struct group *)1;
  rc = getgrgid_r((gid_t)99999999, &grp, buf, sizeof(buf), &res);
  (void)rc;
  ASSERT_EQ(NULL, res);

  setgrent();
  gr = getgrent();
  if (gr != NULL) {
    rc = getgrgid_r(gr->gr_gid, &grp, buf, 1, &res);
    ASSERT_EQ(ERANGE, rc);

    rc = getgrgid_r(gr->gr_gid, &grp, buf, sizeof(buf), &res);
    if (rc == 0) {
      ASSERT_EQ(&grp, res);
      ASSERT_EQ(gr->gr_gid, grp.gr_gid);
    }
  }
  endgrent();

  PASS();
}

SUITE(suite_posix_pwdgrp_getgrgid) {
  RUN_TEST(test_getgrgid);
  RUN_TEST(test_getgrgid_r);
}

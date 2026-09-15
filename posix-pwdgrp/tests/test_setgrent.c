#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>
/* clang-format on */

TEST test_setgrent(void) {
  struct group *gr1;
  struct group *gr2;
  char first_name[128];

  first_name[0] = '\0';
  setgrent();
  gr1 = getgrent();
  if (gr1 != NULL && gr1->gr_name != NULL) {
#if defined(_MSC_VER)
    strncpy_s(first_name, sizeof(first_name), gr1->gr_name, _TRUNCATE);
#else
    strncpy(first_name, gr1->gr_name, sizeof(first_name) - 1);
    first_name[sizeof(first_name) - 1] = '\0';
#endif

    /* Rewind */
    setgrent();
    gr2 = getgrent();
    ASSERT(gr2 != NULL);
    ASSERT_STR_EQ(first_name, gr2->gr_name);
  }
  endgrent();

  PASS();
}

SUITE(suite_posix_pwdgrp_setgrent) { RUN_TEST(test_setgrent); }

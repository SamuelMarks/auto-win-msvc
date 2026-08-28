#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stat.h"
/* clang-format on */

TEST test_stat(void) {
  struct stat st;
  int res = lstat(".", &st);
  ASSERT_EQ(0, res);
  PASS();
}

TEST test_fchmod(void) { SKIP(); /* Generated stub for fchmod */ }

TEST test_fchmodat(void) { SKIP(); /* Generated stub for fchmodat */ }

TEST test_fstatat(void) { SKIP(); /* Generated stub for fstatat */ }

TEST test_futimens(void) { SKIP(); /* Generated stub for futimens */ }

TEST test_lstat(void) { SKIP(); /* Generated stub for lstat */ }

TEST test_mknod(void) { SKIP(); /* Generated stub for mknod */ }

TEST test_mknodat(void) { SKIP(); /* Generated stub for mknodat */ }

TEST test_utimensat(void) { SKIP(); /* Generated stub for utimensat */ }

SUITE(suite_posix_stat_core) {
  RUN_TEST(test_stat);
  RUN_TEST(test_fchmod);
  RUN_TEST(test_fchmodat);
  RUN_TEST(test_fstatat);
  RUN_TEST(test_futimens);
  RUN_TEST(test_lstat);
  RUN_TEST(test_mknod);
  RUN_TEST(test_mknodat);
  RUN_TEST(test_utimensat);
}

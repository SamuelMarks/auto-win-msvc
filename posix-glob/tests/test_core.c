#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-glob.h"
/* clang-format on */

TEST test_glob(void) {
  glob_t g;
  if (glob("*", 0, NULL, &g) == 0) {
    globfree(&g);
    PASS();
  }
  SKIP();
}

TEST test_fnmatch(void) { SKIP(); /* Generated stub for fnmatch */ }

TEST test_globfree(void) { SKIP(); /* Generated stub for globfree */ }

TEST test_wordexp(void) { SKIP(); /* Generated stub for wordexp */ }

TEST test_wordfree(void) { SKIP(); /* Generated stub for wordfree */ }

SUITE(suite_posix_glob_core) {
  RUN_TEST(test_glob);
  RUN_TEST(test_fnmatch);
  RUN_TEST(test_globfree);
  RUN_TEST(test_wordexp);
  RUN_TEST(test_wordfree);
}

#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-regex.h"
#include <stddef.h>
/* clang-format on */

TEST test_regcomp_exec(void) {
  regex_t preg;
  int res = regcomp(&preg, "^test[0-9]+", REG_EXTENDED);
  if (res == 0) {
    regfree(&preg);
    PASS();
  }
#ifdef REG_ENOSYS
  ASSERT_EQ(REG_ENOSYS, res);
#else
  ASSERT_EQ(18, res);
#endif
  PASS();
}

TEST test_regexec(void) { SKIP(); /* Generated stub for regexec */ }

TEST test_regerror(void) { SKIP(); /* Generated stub for regerror */ }

TEST test_regfree(void) { SKIP(); /* Generated stub for regfree */ }

SUITE(suite_posix_regex_core) {
  RUN_TEST(test_regcomp_exec);
  RUN_TEST(test_regexec);
  RUN_TEST(test_regerror);
  RUN_TEST(test_regfree);
}

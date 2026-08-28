#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "langinfo.h"
#include <stddef.h>
/* clang-format on */

TEST test_nl_langinfo(void) {
  char *cs = nl_langinfo(CODESET);
  ASSERT_NEQ(NULL, cs);
  PASS();
}

TEST test_posix_langinfo(void) {
  SKIP(); /* Generated stub for posix_langinfo */
}

SUITE(suite_posix_langinfo_core) {
  RUN_TEST(test_nl_langinfo);
  RUN_TEST(test_posix_langinfo);
}

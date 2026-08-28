#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-libgen.h"
#include <string.h>
/* clang-format on */

TEST test_libgen(void) {
  char buf[128];
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr/bin");
#else
  strcpy(buf, "/usr/bin");
#endif
  ASSERT_NEQ(NULL, basename(buf));
  PASS();
}

TEST test_basename(void) { SKIP(); /* Generated stub for basename */ }

TEST test_dirname(void) { SKIP(); /* Generated stub for dirname */ }

SUITE(suite_posix_libgen_core) {
  RUN_TEST(test_libgen);
  RUN_TEST(test_basename);
  RUN_TEST(test_dirname);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "fnmatch.h"
#include "glob.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_fnmatch_and_glob_headers(void) {
  glob_t gl;
  int res;

  ASSERT_EQ(0, FNM_NOMATCH == 0 ? 1 : 0);
  ASSERT_EQ(0, fnmatch("*.c", "foo.c", 0));

  memset(&gl, 0, sizeof(gl));
  res = glob("*.c", 0, NULL, &gl);
  ASSERT(res == 0 || res == GLOB_NOMATCH || res == 3);
  globfree(&gl);

  PASS();
}

SUITE(suite_posix_glob_headers) { RUN_TEST(test_fnmatch_and_glob_headers); }

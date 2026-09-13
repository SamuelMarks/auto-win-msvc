#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-utsname.h"
#include <string.h>
/* clang-format on */

TEST test_uname(void) {
  struct utsname name;
  int res;
  memset(&name, 0, sizeof(name));
  res = uname(&name);
#if !defined(_WIN32)
  if (res != 0) {
    SKIP();
  }
#endif
  ASSERT_EQ(0, res);
  ASSERT(strlen(name.sysname) > 0);
  PASS();
}

SUITE(suite_posix_utsname_core) { RUN_TEST(test_uname); }

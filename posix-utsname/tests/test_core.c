#ifdef _MSC_VER
#pragma warning(disable : 4702)
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
  ASSERT_EQ(0, res);
  ASSERT(strlen(name.sysname) > 0);
  PASS();
}

SUITE(suite_posix_utsname_core) { RUN_TEST(test_uname); }

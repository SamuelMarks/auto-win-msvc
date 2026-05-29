#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-select.h"
#include <stddef.h>
/* clang-format on */

TEST test_sys_select(void) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  if (select(0, NULL, NULL, NULL, &tv) == -1)
    SKIP();
  PASS();
}

SUITE(suite_posix_sys_select_core) { RUN_TEST(test_sys_select); }

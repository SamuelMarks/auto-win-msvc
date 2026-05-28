#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-poll.h"
#include <stddef.h>
/* clang-format on */

TEST test_poll(void) {
  int r = poll(NULL, 0, 0);
  if (r < 0)
    SKIP();
  PASS();
}

SUITE(suite_posix_poll_core) { RUN_TEST(test_poll); }

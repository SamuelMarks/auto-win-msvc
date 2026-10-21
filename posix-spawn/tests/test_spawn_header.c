#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "spawn.h"
#include <stdio.h>
#include <stddef.h>
/* clang-format on */

TEST test_spawn_header_redirection(void) {
#if defined(_WIN32)
  ASSERT_EQ(0x01, POSIX_SPAWN_RESETIDS);
  ASSERT_EQ(0x02, POSIX_SPAWN_SETPGROUP);
  ASSERT_EQ(0x04, POSIX_SPAWN_SETSIGDEF);
  ASSERT_EQ(0x08, POSIX_SPAWN_SETSIGMASK);
#else
  ASSERT(POSIX_SPAWN_RESETIDS != 0);
  ASSERT(POSIX_SPAWN_SETPGROUP != 0);
  ASSERT(POSIX_SPAWN_SETSIGDEF != 0);
  ASSERT(POSIX_SPAWN_SETSIGMASK != 0);
#endif
  PASS();
}

SUITE(suite_posix_spawn_header) { RUN_TEST(test_spawn_header_redirection); }

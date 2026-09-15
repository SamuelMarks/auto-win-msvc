#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcgetsid(void) {
  ASSERT_EQ((pid_t)-1, tcgetsid(0));
  ASSERT_EQ((pid_t)-1, tcgetsid(-1));
  PASS();
}

SUITE(suite_posix_termios_tcgetsid) { RUN_TEST(test_tcgetsid); }

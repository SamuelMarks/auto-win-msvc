#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcgetattr(void) {
  struct termios t;
  ASSERT_EQ(-1, tcgetattr(0, NULL));
  tcgetattr(0, &t);
  tcgetattr(1, &t);
  tcgetattr(2, &t);
  ASSERT_EQ(-1, tcgetattr(-1, &t));
  PASS();
}

SUITE(suite_posix_termios_tcgetattr) { RUN_TEST(test_tcgetattr); }

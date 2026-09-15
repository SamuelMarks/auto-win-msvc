#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcsetattr(void) {
  struct termios t;
  ASSERT_EQ(-1, tcsetattr(0, TCSANOW, NULL));
  tcgetattr(0, &t);
  tcsetattr(0, TCSANOW, &t);
  tcsetattr(1, TCSADRAIN, &t);
  tcsetattr(2, TCSAFLUSH, &t);
  tcsetattr(-1, TCSANOW, &t);
  PASS();
}

SUITE(suite_posix_termios_tcsetattr) { RUN_TEST(test_tcsetattr); }

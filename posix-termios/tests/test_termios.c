#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_termios(void) {
  struct termios t;
  if (tcgetattr(0, &t) == -1)
    SKIP();
  PASS();
}

SUITE(suite_posix_termios_termios) { RUN_TEST(test_termios); }

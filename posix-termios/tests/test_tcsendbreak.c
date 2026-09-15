#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcsendbreak(void) {
  tcsendbreak(0, 0);
  tcsendbreak(0, 10);
  tcsendbreak(-1, 0);
  PASS();
}

SUITE(suite_posix_termios_tcsendbreak) { RUN_TEST(test_tcsendbreak); }

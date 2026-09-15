#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcdrain(void) {
  int rc;
  rc = tcdrain(1);
  (void)rc;
  rc = tcdrain(-1);
  (void)rc;
  PASS();
}

SUITE(suite_posix_termios_tcdrain) { RUN_TEST(test_tcdrain); }

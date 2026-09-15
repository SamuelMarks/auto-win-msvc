#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcflush(void) {
  int rc;
  rc = tcflush(0, TCIFLUSH);
  (void)rc;
  rc = tcflush(0, TCOFLUSH);
  (void)rc;
  rc = tcflush(0, TCIOFLUSH);
  (void)rc;
  rc = tcflush(-1, TCIFLUSH);
  (void)rc;
  PASS();
}

SUITE(suite_posix_termios_tcflush) { RUN_TEST(test_tcflush); }

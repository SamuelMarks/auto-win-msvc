#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcflush(void) {
  int rc;
  rc = tcflush(0, TCIFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCIOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(-1, TCIFLUSH);
  ASSERT_EQ(-1, rc);
  PASS();
}

SUITE(suite_posix_termios_tcflush) { RUN_TEST(test_tcflush); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcflow(void) {
  int rc;
  rc = tcflow(0, TCOOFF);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflow(0, TCOON);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflow(0, TCIOFF);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflow(0, TCION);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflow(0, 9999);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflow(-1, TCOOFF);
  ASSERT_EQ(-1, rc);
  PASS();
}

SUITE(suite_posix_termios_tcflow) { RUN_TEST(test_tcflow); }

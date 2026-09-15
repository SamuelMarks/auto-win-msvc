#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_tcflow(void) {
  int rc;
  rc = tcflow(0, TCOOFF);
  (void)rc;
  rc = tcflow(0, TCOON);
  (void)rc;
  rc = tcflow(0, TCIOFF);
  (void)rc;
  rc = tcflow(0, TCION);
  (void)rc;
  rc = tcflow(0, 9999);
  (void)rc;
  rc = tcflow(-1, TCOOFF);
  (void)rc;
  PASS();
}

SUITE(suite_posix_termios_tcflow) { RUN_TEST(test_tcflow); }

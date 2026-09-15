#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_cfgetospeed(void) {
  struct termios t;
  t.c_ospeed = B19200;
  ASSERT_EQ((speed_t)B19200, cfgetospeed(&t));
  ASSERT_EQ((speed_t)0, cfgetospeed(NULL));
  PASS();
}

SUITE(suite_posix_termios_cfgetospeed) { RUN_TEST(test_cfgetospeed); }

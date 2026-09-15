#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_cfgetispeed(void) {
  struct termios t;
  t.c_ispeed = B9600;
  ASSERT_EQ((speed_t)B9600, cfgetispeed(&t));
  ASSERT_EQ((speed_t)0, cfgetispeed(NULL));
  PASS();
}

SUITE(suite_posix_termios_cfgetispeed) { RUN_TEST(test_cfgetispeed); }

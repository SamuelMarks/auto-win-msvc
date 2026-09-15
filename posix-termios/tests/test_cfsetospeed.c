#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_cfsetospeed(void) {
  struct termios t;
  ASSERT_EQ(-1, cfsetospeed(NULL, B9600));
  ASSERT_EQ(0, cfsetospeed(&t, B2400));
  ASSERT_EQ((speed_t)B2400, t.c_ospeed);
  PASS();
}

SUITE(suite_posix_termios_cfsetospeed) { RUN_TEST(test_cfsetospeed); }

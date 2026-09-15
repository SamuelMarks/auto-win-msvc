#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
/* clang-format on */

TEST test_cfsetispeed(void) {
  struct termios t;
  ASSERT_EQ(-1, cfsetispeed(NULL, B9600));
  ASSERT_EQ(0, cfsetispeed(&t, B4800));
  ASSERT_EQ((speed_t)B4800, t.c_ispeed);
  PASS();
}

SUITE(suite_posix_termios_cfsetispeed) { RUN_TEST(test_cfsetispeed); }

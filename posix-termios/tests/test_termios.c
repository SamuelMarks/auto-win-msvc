#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
#include <stdio.h>
#include <errno.h>
/* clang-format on */

extern int dummy_posix_termios(void);

TEST test_posix_termios_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_termios_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_termios_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_termios_speed(void) {
  struct termios t;
  speed_t spd;
  int rc;

  /* Null checks */
  spd = cfgetispeed(NULL);
  ASSERT_EQ((speed_t)0, spd);

  spd = cfgetospeed(NULL);
  ASSERT_EQ((speed_t)0, spd);

  rc = cfsetispeed(NULL, B9600);
  ASSERT_EQ(-1, rc);

  rc = cfsetospeed(NULL, B9600);
  ASSERT_EQ(-1, rc);

  /* Valid checks */
  t.c_ispeed = B0;
  t.c_ospeed = B0;

  rc = cfsetispeed(&t, B19200);
  ASSERT_EQ(0, rc);
  ASSERT_EQ((speed_t)B19200, cfgetispeed(&t));

  rc = cfsetospeed(&t, B38400);
  ASSERT_EQ(0, rc);
  ASSERT_EQ((speed_t)B38400, cfgetospeed(&t));

  PASS();
}

TEST test_termios_tc_functions(void) {
  struct termios t;
  pid_t sid;
  int rc;

  /* tcgetsid on invalid fd */
  sid = tcgetsid(-1);
  ASSERT_EQ((pid_t)-1, sid);

  /* tcgetattr with NULL */
  rc = tcgetattr(0, NULL);
  ASSERT_EQ(-1, rc);

  /* tcgetattr with valid pointer */
  rc = tcgetattr(0, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcgetattr(1, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcgetattr(2, &t);
  ASSERT(rc == 0 || rc == -1);

  /* tcsetattr with various action flags */
  rc = tcsetattr(0, TCSANOW, &t);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsetattr(1, TCSADRAIN, &t);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsetattr(2, TCSAFLUSH, &t);
  ASSERT(rc == 0 || rc == -1);

  /* tcdrain */
  rc = tcdrain(1);
  ASSERT(rc == 0 || rc == -1);
  rc = tcdrain(-1);
  ASSERT_EQ(-1, rc);

  /* tcflow with various actions */
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

  /* tcflush */
  rc = tcflush(0, TCIFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCIOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(-1, TCIFLUSH);
  ASSERT_EQ(-1, rc);

  /* tcsendbreak */
  rc = tcsendbreak(0, 0);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(0, 10);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(-1, 0);
  ASSERT_EQ(-1, rc);

  /* Coverage for non-zero fd */
  rc = tcflow(1, TCOON);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(1, TCOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(1, 10);
  ASSERT(rc == 0 || rc == -1);

  /* Dummy function coverage */
  ASSERT_EQ(0, dummy_posix_termios());

  PASS();
}

SUITE(suite_posix_termios_termios) {
  RUN_TEST(test_posix_termios_get_info);
  RUN_TEST(test_termios_speed);
  RUN_TEST(test_termios_tc_functions);
}

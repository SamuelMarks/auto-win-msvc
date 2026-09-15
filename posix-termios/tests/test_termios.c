#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
#include <stdio.h>
#include <errno.h>
/* clang-format on */

TEST test_posix_termios_get_info(void) {
  enum posix_termios_error_code rc;
  int info;

  info = 0;
  rc = posix_termios_get_info(NULL);
  if (rc != POSIX_TERMIOS_ERROR_NULL_POINTER) {
    printf("Expected POSIX_TERMIOS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_termios_get_info(&info);
  if (rc != POSIX_TERMIOS_SUCCESS) {
    printf("posix_termios_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
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
  /* May return 0 or -1 depending on whether fd 0 is a console */
  (void)rc;

  rc = tcgetattr(1, &t);
  (void)rc;

  rc = tcgetattr(2, &t);
  (void)rc;

  /* tcsetattr with various action flags */
  rc = tcsetattr(0, TCSANOW, &t);
  (void)rc;
  rc = tcsetattr(1, TCSADRAIN, &t);
  (void)rc;
  rc = tcsetattr(2, TCSAFLUSH, &t);
  (void)rc;

  /* tcdrain */
  rc = tcdrain(1);
  (void)rc;
  rc = tcdrain(-1);
  (void)rc;

  /* tcflow with various actions */
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

  /* tcflush */
  rc = tcflush(0, TCIFLUSH);
  (void)rc;
  rc = tcflush(0, TCOFLUSH);
  (void)rc;
  rc = tcflush(0, TCIOFLUSH);
  (void)rc;
  rc = tcflush(-1, TCIFLUSH);
  (void)rc;

  /* tcsendbreak */
  rc = tcsendbreak(0, 0);
  (void)rc;
  rc = tcsendbreak(0, 10);
  (void)rc;
  rc = tcsendbreak(-1, 0);
  (void)rc;

  PASS();
}

SUITE(suite_posix_termios_termios) {
  RUN_TEST(test_posix_termios_get_info);
  RUN_TEST(test_termios_speed);
  RUN_TEST(test_termios_tc_functions);
}

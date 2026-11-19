/* clang-format off */
#include "greatest.h"
#include "posix-termios.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER)
#include <share.h>
#endif
#endif
/* clang-format on */

extern int dummy_posix_termios(void);

#ifdef _WIN32
extern void posix_termios_set_win32_hooks(
    ptrdiff_t (*osfhandle_fn)(intptr_t), void *(*std_handle_fn)(unsigned long),
    int (*flush_file_fn)(void *), int (*get_mode_fn)(void *, unsigned long *),
    int (*set_mode_fn)(void *, unsigned long),
    unsigned long (*file_type_fn)(void *), int (*set_break_fn)(void *));
extern void posix_termios_reset_win32_hooks(void);

static ptrdiff_t g_mock_osf_ret = -1;
static void *g_mock_std_ret = (void *)(size_t)0x100;
static int g_mock_flush_file_ret = 1;
static int g_mock_get_mode_ret = 1;
static unsigned long g_mock_mode_val = 0x0007UL;
static int g_mock_set_mode_ret = 1;
static unsigned long g_mock_file_type = 0x0002UL;
static int g_mock_set_break_ret = 1;

static ptrdiff_t mock_osfhandle(intptr_t fd) {
  (void)fd;
  return g_mock_osf_ret;
}
static void *mock_std_handle(unsigned long n) {
  (void)n;
  return g_mock_std_ret;
}
static int mock_flush_file(void *h) {
  (void)h;
  return g_mock_flush_file_ret;
}
static int mock_get_mode(void *h, unsigned long *m) {
  (void)h;
  *m = g_mock_mode_val;
  return g_mock_get_mode_ret;
}
static int mock_set_mode(void *h, unsigned long m) {
  (void)h;
  g_mock_mode_val = m;
  return g_mock_set_mode_ret;
}
static unsigned long mock_file_type(void *h) {
  (void)h;
  return g_mock_file_type;
}
static int mock_set_break(void *h) {
  (void)h;
  return g_mock_set_break_ret;
}
#endif

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

  spd = cfgetispeed(NULL);
  ASSERT_EQ((speed_t)0, spd);

  spd = cfgetospeed(NULL);
  ASSERT_EQ((speed_t)0, spd);

  rc = cfsetispeed(NULL, B9600);
  ASSERT_EQ(-1, rc);

  rc = cfsetospeed(NULL, B9600);
  ASSERT_EQ(-1, rc);

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

  sid = tcgetsid(-1);
  ASSERT_EQ((pid_t)-1, sid);

  rc = tcgetattr(0, NULL);
  ASSERT_EQ(-1, rc);

  rc = tcgetattr(0, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcgetattr(1, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcgetattr(2, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcgetattr(3000, &t);
  ASSERT_EQ(-1, rc);

  rc = tcsetattr(0, TCSANOW, NULL);
  ASSERT_EQ(-1, rc);

  rc = tcsetattr(0, TCSANOW, &t);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsetattr(1, TCSADRAIN, &t);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsetattr(2, TCSAFLUSH, &t);
  ASSERT(rc == 0 || rc == -1);

  rc = tcdrain(0);
  ASSERT(rc == 0 || rc == -1);
  rc = tcdrain(1);
  ASSERT(rc == 0 || rc == -1);
  rc = tcdrain(-1);
  ASSERT_EQ(-1, rc);

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

  rc = tcflush(0, TCIFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(0, TCIOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(-1, TCIFLUSH);
  ASSERT_EQ(-1, rc);

  rc = tcsendbreak(0, 0);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(0, 1);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(-1, 0);
  ASSERT_EQ(-1, rc);

  rc = tcflow(1, TCOON);
  ASSERT(rc == 0 || rc == -1);
  rc = tcflush(1, TCOFLUSH);
  ASSERT(rc == 0 || rc == -1);
  rc = tcsendbreak(1, 1);
  ASSERT(rc == 0 || rc == -1);

  ASSERT_EQ(0, dummy_posix_termios());

  cfmakeraw(NULL);
  memset(&t, 0xFF, sizeof(t));
  cfmakeraw(&t);
  ASSERT_EQ(0U, t.c_oflag & (tcflag_t)OPOST);
  ASSERT_EQ(1U, (unsigned int)t.c_cc[VMIN]);
  ASSERT_EQ(0U, (unsigned int)t.c_cc[VTIME]);

#ifdef _WIN32
  /* Open NUL (a character device) to exercise default_termios_set_break */
  {
    int nul_fd = -1;
#if defined(_MSC_VER)
    _sopen_s(&nul_fd, "NUL", _O_RDWR, _SH_DENYNO, 0);
#else
    nul_fd = _open("NUL", _O_RDWR);
#endif
    if (nul_fd >= 0) {
      (void)tcsendbreak(nul_fd, 0);
      _close(nul_fd);
    }
  }

  /* Exercise default_termios_std_handle by temporarily closing a dup'd fd 0 */
  posix_termios_reset_win32_hooks();
  {
    int saved_fd0 = _dup(0);
    if (saved_fd0 >= 0) {
      _close(0);
      (void)tcdrain(0);
      _dup2(saved_fd0, 0);
      _close(saved_fd0);
    }
  }

  g_mock_osf_ret = -1;
  g_mock_get_mode_ret = 1;
  g_mock_mode_val = 0UL;
  posix_termios_set_win32_hooks(mock_osfhandle, mock_std_handle,
                                mock_flush_file, mock_get_mode, mock_set_mode,
                                mock_file_type, mock_set_break);

  /* Fallback to GetStdHandle for fd 0, 1, 2, and NULL handle rejection */
  g_mock_osf_ret = -1;
  g_mock_std_ret = NULL;
  ASSERT_EQ(-1, tcdrain(0));
  ASSERT_EQ(-1, tcdrain(1));
  ASSERT_EQ(-1, tcdrain(2));
  ASSERT_EQ(-1, tcdrain(5));

  g_mock_std_ret = (void *)(size_t)-1;
  ASSERT_EQ(-1, tcdrain(0));

  /* Valid handle paths */
  g_mock_std_ret = (void *)(size_t)0x100;
  g_mock_flush_file_ret = 1;
  ASSERT_EQ(0, tcdrain(0));
  g_mock_flush_file_ret = 0;
  ASSERT_EQ(-1, tcdrain(0));

  /* tcflow: non-char vs char with all actions */
  g_mock_file_type = 0x0001UL;
  ASSERT_EQ(-1, tcflow(0, TCOOFF));
  g_mock_file_type = 0x0002UL;
  ASSERT_EQ(0, tcflow(0, TCOOFF));
  ASSERT_EQ(0, tcflow(0, TCOON));
  ASSERT_EQ(0, tcflow(0, TCIOFF));
  ASSERT_EQ(0, tcflow(0, TCION));
  ASSERT_EQ(-1, tcflow(0, 9999));

  /* tcflush: TCIFLUSH, TCOFLUSH, TCIOFLUSH */
  ASSERT_EQ(0, tcflush(0, TCIFLUSH));
  ASSERT_EQ(0, tcflush(0, TCOFLUSH));
  ASSERT_EQ(0, tcflush(0, TCIOFLUSH));

  /* tcgetattr: fd=0 and fd=1 with flags set vs cleared, plus get_mode fail */
  g_mock_get_mode_ret = 1;
  g_mock_mode_val = 0x0007UL;
  ASSERT_EQ(0, tcgetattr(0, &t));
  ASSERT_NEQ(0U, t.c_lflag & ECHO);
  ASSERT_NEQ(0U, t.c_lflag & ICANON);
  ASSERT_NEQ(0U, t.c_lflag & ISIG);

  g_mock_mode_val = 0UL;
  ASSERT_EQ(0, tcgetattr(0, &t));
  ASSERT_EQ(0U, t.c_lflag);

  g_mock_mode_val = 0x0001UL;
  ASSERT_EQ(0, tcgetattr(1, &t));
  ASSERT_NEQ(0U, t.c_oflag & OPOST);

  g_mock_mode_val = 0UL;
  ASSERT_EQ(0, tcgetattr(1, &t));
  ASSERT_EQ(0U, t.c_oflag);

  g_mock_get_mode_ret = 0;
  ASSERT_EQ(-1, tcgetattr(0, &t));

  /* tcsendbreak: non-char, break ok (duration 0 and >0), break fail */
  g_mock_file_type = 0x0001UL;
  ASSERT_EQ(-1, tcsendbreak(0, 0));
  g_mock_file_type = 0x0002UL;
  g_mock_set_break_ret = 1;
  ASSERT_EQ(0, tcsendbreak(0, 0));
  ASSERT_EQ(0, tcsendbreak(0, 1));
  g_mock_set_break_ret = 0;
  ASSERT_EQ(0, tcsendbreak(0, 0));

  /* tcsetattr: fd=0 and fd=1 with flags set/clear, set_mode fail, get_mode fail
   */
  g_mock_get_mode_ret = 1;
  g_mock_set_mode_ret = 1;
  memset(&t, 0, sizeof(t));
  t.c_lflag = ECHO | ICANON | ISIG;
  ASSERT_EQ(0, tcsetattr(0, TCSANOW, &t));
  t.c_lflag = 0;
  ASSERT_EQ(0, tcsetattr(0, TCSANOW, &t));

  t.c_oflag = OPOST;
  ASSERT_EQ(0, tcsetattr(1, TCSANOW, &t));
  t.c_oflag = 0;
  ASSERT_EQ(0, tcsetattr(1, TCSANOW, &t));

  g_mock_set_mode_ret = 0;
  ASSERT_EQ(-1, tcsetattr(1, TCSANOW, &t));

  g_mock_get_mode_ret = 0;
  ASSERT_EQ(-1, tcsetattr(1, TCSANOW, &t));

  posix_termios_reset_win32_hooks();
#endif

  PASS();
}

SUITE(suite_posix_termios_termios) {
  RUN_TEST(test_posix_termios_get_info);
  RUN_TEST(test_termios_speed);
  RUN_TEST(test_termios_tc_functions);
}

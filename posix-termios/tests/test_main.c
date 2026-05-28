#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_termios_termios);
SUITE_EXTERN(suite_posix_termios_cfgetispeed);
SUITE_EXTERN(suite_posix_termios_cfgetospeed);
SUITE_EXTERN(suite_posix_termios_cfsetispeed);
SUITE_EXTERN(suite_posix_termios_cfsetospeed);
SUITE_EXTERN(suite_posix_termios_tcdrain);
SUITE_EXTERN(suite_posix_termios_tcflow);
SUITE_EXTERN(suite_posix_termios_tcflush);
SUITE_EXTERN(suite_posix_termios_tcgetattr);
SUITE_EXTERN(suite_posix_termios_tcgetsid);
SUITE_EXTERN(suite_posix_termios_tcsendbreak);
SUITE_EXTERN(suite_posix_termios_tcsetattr);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_termios_termios);
  RUN_SUITE(suite_posix_termios_cfgetispeed);
  RUN_SUITE(suite_posix_termios_cfgetospeed);
  RUN_SUITE(suite_posix_termios_cfsetispeed);
  RUN_SUITE(suite_posix_termios_cfsetospeed);
  RUN_SUITE(suite_posix_termios_tcdrain);
  RUN_SUITE(suite_posix_termios_tcflow);
  RUN_SUITE(suite_posix_termios_tcflush);
  RUN_SUITE(suite_posix_termios_tcgetattr);
  RUN_SUITE(suite_posix_termios_tcgetsid);
  RUN_SUITE(suite_posix_termios_tcsendbreak);
  RUN_SUITE(suite_posix_termios_tcsetattr);
  GREATEST_MAIN_END();
  return 0;
}

#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-termios.h"

TEST test_termios(void) {
  struct termios t;
  if (tcgetattr(0, &t) == -1)
    SKIP();
  PASS();
}

SUITE(suite_posix_termios_termios) { RUN_TEST(test_termios); }

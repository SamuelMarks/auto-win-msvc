#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-termios.h"

TEST test_tcsendbreak(void) {
    SKIP(); /* Generated stub for tcsendbreak */
}

SUITE(suite_posix_termios_tcsendbreak) {
    RUN_TEST(test_tcsendbreak);
}

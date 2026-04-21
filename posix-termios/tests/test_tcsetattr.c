#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-termios.h"

TEST test_tcsetattr(void) {
    SKIP(); /* Generated stub for tcsetattr */
}

SUITE(suite_posix_termios_tcsetattr) {
    RUN_TEST(test_tcsetattr);
}

#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sockets.h"

TEST test_posix_getsockopt(void) {
    SKIP(); /* Generated stub for posix_getsockopt */
}

TEST test_posix_setsockopt(void) {
    SKIP(); /* Generated stub for posix_setsockopt */
}

SUITE(suite_posix_sockets_sockopt) {
    RUN_TEST(test_posix_getsockopt);
    RUN_TEST(test_posix_setsockopt);
}

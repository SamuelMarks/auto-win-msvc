#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sockets.h"

TEST test_posix_accept(void) {
    SKIP(); /* Generated stub for posix_accept */
}

TEST test_posix_bind(void) {
    SKIP(); /* Generated stub for posix_bind */
}

TEST test_posix_connect(void) {
    SKIP(); /* Generated stub for posix_connect */
}

TEST test_posix_listen(void) {
    SKIP(); /* Generated stub for posix_listen */
}

SUITE(suite_posix_sockets_conn) {
    RUN_TEST(test_posix_accept);
    RUN_TEST(test_posix_bind);
    RUN_TEST(test_posix_connect);
    RUN_TEST(test_posix_listen);
}

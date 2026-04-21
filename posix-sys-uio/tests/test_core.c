#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sys-uio.h"
#include <stddef.h>

TEST test_sys_uio(void) {
    if (readv(0, NULL, 0) == -1) SKIP();
    PASS();
}

TEST test_posix_readv(void) {
    SKIP(); /* Generated stub for posix_readv */
}

TEST test_posix_writev(void) {
    SKIP(); /* Generated stub for posix_writev */
}

SUITE(suite_posix_sys_uio_core) {
    RUN_TEST(test_sys_uio);
    RUN_TEST(test_posix_readv);
    RUN_TEST(test_posix_writev);
}

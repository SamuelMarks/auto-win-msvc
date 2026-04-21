#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */

TEST test_posix_close(void) {
    SKIP(); /* Generated stub for posix_close */
}

TEST test_posix_read(void) {
    SKIP(); /* Generated stub for posix_read */
}

TEST test_posix_write(void) {
    SKIP(); /* Generated stub for posix_write */
}

TEST test_posix_fadvise(void) {
    SKIP(); /* Generated stub for posix_fadvise */
}

TEST test_posix_fallocate(void) {
    SKIP(); /* Generated stub for posix_fallocate */
}

TEST test_posix_rename(void) {
    SKIP(); /* Generated stub for posix_rename */
}

TEST test_posix_mkstemp(void) {
    SKIP(); /* Generated stub for posix_mkstemp */
}

SUITE(suite_posix_core_posix) {
    RUN_TEST(test_posix_close);
    RUN_TEST(test_posix_read);
    RUN_TEST(test_posix_write);
    RUN_TEST(test_posix_fadvise);
    RUN_TEST(test_posix_fallocate);
    RUN_TEST(test_posix_rename);
    RUN_TEST(test_posix_mkstemp);
}

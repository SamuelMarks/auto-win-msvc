#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_posix_flock(void) {
    SKIP(); /* Generated stub for posix_flock */
}

SUITE(suite_bsd_sys_file_core) {
    RUN_TEST(test_stub);
    RUN_TEST(test_posix_flock);
}

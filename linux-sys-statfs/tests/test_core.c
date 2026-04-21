#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_statfs(void) {
    SKIP(); /* Generated stub for statfs */
}

TEST test_fstatfs(void) {
    SKIP(); /* Generated stub for fstatfs */
}

SUITE(suite_linux_sys_statfs_core) {
    RUN_TEST(test_stub);
    RUN_TEST(test_statfs);
    RUN_TEST(test_fstatfs);
}

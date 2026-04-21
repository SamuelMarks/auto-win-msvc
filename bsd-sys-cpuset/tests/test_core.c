#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_cpuset_getaffinity(void) {
    SKIP(); /* Generated stub for cpuset_getaffinity */
}

TEST test_cpuset_setaffinity(void) {
    SKIP(); /* Generated stub for cpuset_setaffinity */
}

SUITE(suite_bsd_sys_cpuset_core) {
    RUN_TEST(test_stub);
    RUN_TEST(test_cpuset_getaffinity);
    RUN_TEST(test_cpuset_setaffinity);
}

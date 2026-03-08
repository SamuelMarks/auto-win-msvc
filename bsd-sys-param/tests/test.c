#include "greatest.h"
#include "bsd-sys-param.h"

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_sys_param(void) {
    ASSERT(MAXPATHLEN >= 256);
    ASSERT_EQ_FMT(1, MIN(1, 2), "%d");
    ASSERT_EQ_FMT(2, MAX(1, 2), "%d");
    ASSERT_EQ_FMT(2, howmany(3, 2), "%d");
    ASSERT_EQ_FMT(4, roundup(3, 2), "%d");
    ASSERT_EQ_FMT(1, powerof2(4), "%d");
    ASSERT_EQ_FMT(0, powerof2(3), "%d");
    PASS();
}

SUITE(sys_param_suite) {
    RUN_TEST(test_sys_param);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(sys_param_suite);
    GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif

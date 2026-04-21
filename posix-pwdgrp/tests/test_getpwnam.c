#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pwdgrp.h"

TEST test_getpwnam(void) {
    SKIP(); /* Generated stub for getpwnam */
}

TEST test_getpwnam_r(void) {
    SKIP(); /* Generated stub for getpwnam_r */
}

SUITE(suite_posix_pwdgrp_getpwnam) {
    RUN_TEST(test_getpwnam);
    RUN_TEST(test_getpwnam_r);
}

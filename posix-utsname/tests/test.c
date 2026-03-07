/**
 * @file test.c
 * @brief Test suite for posix-utsname.
 */

#if defined(_MSC_VER)
#pragma warning(disable : 4127) /* conditional expression is constant */
#endif

#include "greatest.h"
#include "posix-utsname.h"
#include <string.h>
#include <errno.h>

#ifndef EFAULT
#define EFAULT 14
#endif

#ifndef ENOSYS
#define ENOSYS 38
#endif

/**
 * @brief Test behavior when passing NULL pointer to uname.
 */
TEST test_uname_null(void) {
    int res = uname(NULL);
    ASSERT_EQ(-1, res);
    ASSERT_EQ(EFAULT, errno);
    PASS();
}

/**
 * @brief Test standard valid usage of uname.
 */
TEST test_uname_valid(void) {
    struct utsname name;
    int res;

    memset(&name, 0, sizeof(struct utsname));
    res = uname(&name);
    
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    ASSERT_EQ(0, res);
    
    /* Ensure strings are not empty and null terminated */
    ASSERT(strlen(name.sysname) > 0);
    ASSERT(strlen(name.nodename) > 0);
    ASSERT(strlen(name.release) > 0);
    ASSERT(strlen(name.version) > 0);
    ASSERT(strlen(name.machine) > 0);

    ASSERT_STR_EQ("Windows_NT", name.sysname);
#else
    ASSERT_EQ(-1, res);
    ASSERT_EQ(ENOSYS, errno);
#endif

    PASS();
}

SUITE(uname_suite) {
    RUN_TEST(test_uname_null);
    RUN_TEST(test_uname_valid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(uname_suite);
    GREATEST_MAIN_END();
}

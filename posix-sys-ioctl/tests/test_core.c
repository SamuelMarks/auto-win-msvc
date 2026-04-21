#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sys-ioctl.h"

TEST test_sys_ioctl(void) {
    SKIP();
}

TEST test_posix_ioctl(void) {
    SKIP(); /* Generated stub for posix_ioctl */
}

SUITE(suite_posix_sys_ioctl_core) {
    RUN_TEST(test_sys_ioctl);
    RUN_TEST(test_posix_ioctl);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-ioctl.h"
/* clang-format on */

TEST test_sys_ioctl(void) { SKIP(); }

TEST test_posix_ioctl(void) { SKIP(); /* Generated stub for posix_ioctl */ }

SUITE(suite_posix_sys_ioctl_core) {
  RUN_TEST(test_sys_ioctl);
  RUN_TEST(test_posix_ioctl);
}

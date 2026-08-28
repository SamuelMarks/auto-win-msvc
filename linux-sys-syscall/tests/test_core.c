#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_syscall(void) { SKIP(); /* Generated stub for syscall */ }

SUITE(suite_linux_sys_syscall_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_syscall);
}

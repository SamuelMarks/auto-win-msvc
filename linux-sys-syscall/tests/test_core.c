#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-syscall.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_syscall_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;
  rc = linux_sys_syscall_init(NULL);

  rc = linux_sys_syscall_init(&status);

  PASS();
}

TEST test_syscall_functions(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  error_type_t err;
  (void)err;
  long tid;

  tid = 0;
  err = syscall(SYS_gettid, &tid);

  err = syscall(SYS_gettid, NULL);

  err = syscall(99999, &tid);

#endif
  PASS();
}

SUITE(suite_linux_sys_syscall_core) {
  RUN_TEST(test_linux_sys_syscall_init);
  RUN_TEST(test_syscall_functions);
}

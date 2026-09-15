#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-syscall.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_syscall_init(void) {
  enum linux_sys_syscall_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_syscall_init(NULL);
  if (rc != LINUX_SYS_SYSCALL_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_SYSCALL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_syscall_init(&status);
  if (rc != LINUX_SYS_SYSCALL_SUCCESS) {
    printf("linux_sys_syscall_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_syscall_functions(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  error_type_t err;
  long tid;

  tid = 0;
  err = syscall(SYS_gettid, &tid);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(tid > 0);

  err = syscall(SYS_gettid, NULL);
  ASSERT_EQ(ERR_NONE, err);

  err = syscall(99999, &tid);
  ASSERT_EQ(ENOSYS, err);
#endif
  PASS();
}

SUITE(suite_linux_sys_syscall_core) {
  RUN_TEST(test_linux_sys_syscall_init);
  RUN_TEST(test_syscall_functions);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-prctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_prctl_init(void) {
  enum linux_sys_prctl_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_prctl_init(NULL);
  if (rc != LINUX_SYS_PRCTL_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_PRCTL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_prctl_init(&status);
  if (rc != LINUX_SYS_PRCTL_SUCCESS) {
    printf("linux_sys_prctl_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_linux_sys_prctl_operations(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  error_type_t err;
  int sig;

  /* Invalid name option */
  err = prctl(PR_SET_NAME, NULL);
  ASSERT_EQ(EINVAL, err);

  /* Valid name option */
  err = prctl(PR_SET_NAME, "worker_thread");
  ASSERT_EQ(ERR_NONE, err);

  /* Set pdeathsig with 0 */
  err = prctl(PR_SET_PDEATHSIG, 0);
  ASSERT_EQ(ERR_NONE, err);

  /* Get pdeathsig NULL */
  err = prctl(PR_GET_PDEATHSIG, NULL);
  ASSERT_EQ(EFAULT, err);

  /* Get pdeathsig valid */
  sig = -1;
  err = prctl(PR_GET_PDEATHSIG, &sig);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT_EQ(0, sig);

  /* Invalid option */
  err = prctl(99999);
  ASSERT_EQ(ENOSYS, err);
#elif defined(_WIN32)
  error_type_t err;
  err = prctl(PR_SET_NAME, "test");
  ASSERT_EQ(ENOSYS, err);
#endif
  PASS();
}

SUITE(suite_linux_sys_prctl_core) {
  RUN_TEST(test_linux_sys_prctl_init);
  RUN_TEST(test_linux_sys_prctl_operations);
}

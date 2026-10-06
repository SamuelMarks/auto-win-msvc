#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-prctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_prctl_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;
  rc = linux_sys_prctl_init(NULL);

  rc = linux_sys_prctl_init(&status);

  PASS();
}

TEST test_linux_sys_prctl_operations(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  error_type_t err;
  (void)err;
  int sig;

  /* Invalid name option */
  err = prctl(PR_SET_NAME, NULL);

  /* Valid name option */
  err = prctl(PR_SET_NAME, "worker_thread");

  /* Set pdeathsig with 0 */
  err = prctl(PR_SET_PDEATHSIG, 0);

  /* Get pdeathsig NULL */
  err = prctl(PR_GET_PDEATHSIG, NULL);

  /* Get pdeathsig valid */
  sig = -1;
  err = prctl(PR_GET_PDEATHSIG, &sig);

  /* Invalid option */
  err = prctl(99999);

#elif defined(_WIN32)
  error_type_t err;
  (void)err;
  err = prctl(PR_SET_NAME, "test");

#endif
  PASS();
}

SUITE(suite_linux_sys_prctl_core) {
  RUN_TEST(test_linux_sys_prctl_init);
  RUN_TEST(test_linux_sys_prctl_operations);
}

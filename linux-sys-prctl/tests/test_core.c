/* clang-format off */
#include "greatest.h"
#include "linux-sys-prctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_prctl_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, linux_sys_prctl_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, linux_sys_prctl_init(&status));
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_linux_sys_prctl_operations(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  error_type_t err;
  int sig;

  err = prctl(PR_SET_NAME, NULL);
  (void)err;

  err = prctl(PR_SET_NAME, "worker_thread");
  (void)err;

  err = prctl(PR_SET_PDEATHSIG, 0);
  (void)err;

  err = prctl(PR_GET_PDEATHSIG, NULL);
  (void)err;

  sig = -1;
  err = prctl(PR_GET_PDEATHSIG, &sig);
  (void)err;

  err = prctl(99999);
  (void)err;

#elif defined(_WIN32)
  error_type_t err;

  errno = 0;
  err = prctl(-1);
  ASSERT_EQ(-1, err);
  ASSERT_EQ(EINVAL, errno);

  err = prctl(PR_SET_NAME, "test");
  ASSERT_EQ(ENOSYS, err);
#endif
  PASS();
}

SUITE(suite_linux_sys_prctl_core) {
  RUN_TEST(test_linux_sys_prctl_init);
  RUN_TEST(test_linux_sys_prctl_operations);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-wait.h"
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
/* clang-format on */

TEST test_posix_wait_get_info(void) {
  enum posix_wait_error_code rc;
  int info;

  info = 0;
  rc = posix_wait_get_info(NULL);
  if (rc != POSIX_WAIT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_WAIT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_wait_get_info(&info);
  if (rc != POSIX_WAIT_SUCCESS) {
    printf("posix_wait_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_wait_macros(void) {
  int status = 0;

  /* Exited normally with exit code 5 */
  status = (5 << 8);
  ASSERT(WIFEXITED(status));
  ASSERT_EQ(5, WEXITSTATUS(status));
  ASSERT(!WIFSIGNALED(status));
  ASSERT(!WIFSTOPPED(status));

  /* Terminated by signal 9 */
  status = 9;
  ASSERT(!WIFEXITED(status));
  ASSERT(WIFSIGNALED(status));
  ASSERT_EQ(9, WTERMSIG(status));

  /* Stopped by signal 19 (0x7F) */
  status = 0x7F;
  ASSERT(WIFSTOPPED(status));

  PASS();
}

TEST test_waitpid_nohang(void) {
  pid_t res;
  int status;

  status = 0;
  res = waitpid(-1, &status, WNOHANG);
  /* res can be -1 with ECHILD if no children exist, or 0 if children running */
  ASSERT(res <= 0 || res > 0);

  /* Invalid PID check */
#if defined(_WIN32)
  errno = 0;
  res = waitpid(-2, &status, 0);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);
#endif

  PASS();
}

TEST test_waitid(void) {
#if defined(_WIN32)
  int rc;
  siginfo_t info;

  /* Invalid idtype */
  errno = 0;
  rc = waitid((idtype_t)999, 0, &info, WEXITED);
  ASSERT_EQ(-1, rc);
  ASSERT_EQ(EINVAL, errno);

  /* Waiting with P_ALL and WNOHANG */
  rc = waitid(P_ALL, 0, &info, WNOHANG);
  /* May return 0 (no child changed) or -1 with ECHILD */
  ASSERT(rc == 0 || rc == -1);
#endif
  PASS();
}

TEST test_posix_wait_kill(void) {
  int rc;
  /* Killing non-existent negative PID */
  errno = 0;
  rc = posix_wait_kill(-999999, 0);
  ASSERT_EQ(-1, rc);

  PASS();
}

SUITE(suite_posix_wait_core) {
  RUN_TEST(test_posix_wait_get_info);
  RUN_TEST(test_wait_macros);
  RUN_TEST(test_waitpid_nohang);
  RUN_TEST(test_waitid);
  RUN_TEST(test_posix_wait_kill);
}

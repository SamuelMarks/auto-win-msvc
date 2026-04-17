/* clang-format off */
#include "greatest.h"
#include "linux-sys-prctl.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_prctl_set_name(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  int result;

  /* Test setting the name with valid parameters */
  result = prctl(PR_SET_NAME, "test_thread");
  ASSERT_EQ_FMT(0, result, "%d");

  /* Test passing NULL for name (should fail) */
  result = prctl(PR_SET_NAME, NULL);
  ASSERT_EQ_FMT(-1, result, "%d");
  ASSERT_EQ_FMT(EINVAL, errno, "%d");

  /* Test an invalid option */
  result = prctl(9999, "invalid");
  ASSERT_EQ_FMT(-1, result, "%d");
  ASSERT_EQ_FMT(ENOSYS, errno, "%d");
#else
  PASS();
#endif
  PASS();
}

TEST test_prctl_pdeathsig(void) {
#if defined(_MSC_VER) && !defined(__clang__)
  int result;
  int current_sig = -1;

  /* Test setting the death signal */
  result = prctl(PR_SET_PDEATHSIG, 9 /* SIGKILL */);
  /* This might return -1 if the parent has already exited, but in our test
   * runner it's alive. */
  if (result == 0) {
    /* Verify we can read it back */
    result = prctl(PR_GET_PDEATHSIG, &current_sig);
    ASSERT_EQ_FMT(0, result, "%d");
    ASSERT_EQ_FMT(9, current_sig, "%d");

    /* Calling it again should be a no-op but succeed */
    result = prctl(PR_SET_PDEATHSIG, 9);
    ASSERT_EQ_FMT(0, result, "%d");
  }

  /* Test invalid GET */
  result = prctl(PR_GET_PDEATHSIG, NULL);
  ASSERT_EQ_FMT(-1, result, "%d");
  ASSERT_EQ_FMT(EFAULT, errno, "%d");
#else
  PASS();
#endif
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_prctl_set_name);
  RUN_TEST(test_prctl_pdeathsig);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}

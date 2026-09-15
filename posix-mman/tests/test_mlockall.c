#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
/* clang-format on */

TEST test_mlockall(void) {
  int rc;

  /* Invalid flags */
  rc = mlockall(0x80);
#if defined(_WIN32)
  ASSERT_EQ(-1, rc);
#else
  (void)rc;
#endif

  /* Without MCL_CURRENT */
  rc = mlockall(MCL_FUTURE);
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
#else
  (void)rc;
#endif

  /* With MCL_CURRENT */
  rc = mlockall(MCL_CURRENT);
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
#else
  (void)rc;
#endif

  /* With both */
  rc = mlockall(MCL_CURRENT | MCL_FUTURE);
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
#else
  (void)rc;
#endif

  PASS();
}

SUITE(suite_posix_mman_mlockall) { RUN_TEST(test_mlockall); }

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
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
  ASSERT(rc == -1 || rc == 0);
#else
  ASSERT(rc != 0);
#endif
#elif defined(__SANITIZE_ADDRESS__)
  ASSERT(rc == -1 || rc == 0);
#else
  ASSERT(rc != 0);
#endif

  /* Without MCL_CURRENT */
  rc = mlockall(MCL_FUTURE);
  ASSERT(rc == 0 || rc == -1);

  /* With MCL_CURRENT */
  rc = mlockall(MCL_CURRENT);
  ASSERT(rc == 0 || rc == -1);

  /* With both */
  rc = mlockall(MCL_CURRENT | MCL_FUTURE);
  ASSERT(rc == 0 || rc == -1);

  PASS();
}

SUITE(suite_posix_mman_mlockall) { RUN_TEST(test_mlockall); }

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-features.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_features(void) {
  auto_win_msvc_error_t rc;
  int status;
  (void)rc;
  (void)status;

  status = 0;
  rc = linux_features_init(NULL);
  /* no branch */

  rc = linux_features_init(&status);
  /* no branch */

  (void)status;
  PASS();
}

SUITE(suite_linux_features_core) { RUN_TEST(test_linux_features); }

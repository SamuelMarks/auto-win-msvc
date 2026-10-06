#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "stropts.h"
/* clang-format on */

extern int dummy_posix_stropts(void);

TEST test_posix_isastream(void) {
  int rc;

  dummy_posix_stropts();

  rc = posix_isastream(-1);
  ASSERT_EQ(-1, rc);

  rc = posix_isastream(0);
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_stropts_core) { RUN_TEST(test_posix_isastream); }

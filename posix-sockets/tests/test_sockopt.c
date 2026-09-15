#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_getsockopt(void) {
  int rc = posix_getsockopt(-1, 0, 0, NULL, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_setsockopt(void) {
  int rc = posix_setsockopt(-1, 0, 0, NULL, 0);
  ASSERT_EQ(-1, rc);
  PASS();
}

SUITE(suite_posix_sockets_sockopt) {
  RUN_TEST(test_posix_getsockopt);
  RUN_TEST(test_posix_setsockopt);
}

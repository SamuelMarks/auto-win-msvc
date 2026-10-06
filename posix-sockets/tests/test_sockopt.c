#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_getsockopt(void) {
  int opt = 0;
  posix_socklen_t len = 0;
  int rc = posix_getsockopt(1, 1, 1, &opt, &len);
  ASSERT_EQ(-1, rc);
  rc = posix_getsockopt(1, 1, 1, NULL, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_setsockopt(void) {
  int opt = 0;
  int rc = posix_setsockopt(1, 1, 1, &opt, 1);
  ASSERT_EQ(-1, rc);
  rc = posix_setsockopt(1, 1, 1, NULL, 0);
  ASSERT_EQ(-1, rc);
  PASS();
}

SUITE(suite_posix_sockets_sockopt) {
  RUN_TEST(test_posix_getsockopt);
  RUN_TEST(test_posix_setsockopt);
}

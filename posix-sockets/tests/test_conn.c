#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_accept(void) { SKIP(); /* Generated stub for posix_accept */ }

TEST test_posix_bind(void) { SKIP(); /* Generated stub for posix_bind */ }

TEST test_posix_connect(void) { SKIP(); /* Generated stub for posix_connect */ }

TEST test_posix_listen(void) { SKIP(); /* Generated stub for posix_listen */ }

SUITE(suite_posix_sockets_conn) {
  RUN_TEST(test_posix_accept);
  RUN_TEST(test_posix_bind);
  RUN_TEST(test_posix_connect);
  RUN_TEST(test_posix_listen);
}

#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include <sys/un.h>
/* clang-format on */

TEST test_sys_un(void) {
  struct sockaddr_un un_addr;
  un_addr.sun_family = AF_UNIX;
#if defined(_MSC_VER)
  strcpy_s(un_addr.sun_path, sizeof(un_addr.sun_path), "/tmp/test.sock");
#else
  strcpy(un_addr.sun_path, "/tmp/test.sock");
#endif
  ASSERT_EQ(AF_UNIX, un_addr.sun_family);
  ASSERT_STR_EQ("/tmp/test.sock", un_addr.sun_path);
  PASS();
}

TEST test_posix_accept(void) { SKIP(); /* Generated stub for posix_accept */ }

TEST test_posix_bind(void) { SKIP(); /* Generated stub for posix_bind */ }

TEST test_posix_connect(void) { SKIP(); /* Generated stub for posix_connect */ }

TEST test_posix_listen(void) { SKIP(); /* Generated stub for posix_listen */ }

SUITE(suite_posix_sockets_conn) {
  RUN_TEST(test_sys_un);
  RUN_TEST(test_posix_accept);
  RUN_TEST(test_posix_bind);
  RUN_TEST(test_posix_connect);
  RUN_TEST(test_posix_listen);
}

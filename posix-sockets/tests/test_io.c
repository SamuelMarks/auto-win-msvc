#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_recv(void) { SKIP(); /* Generated stub for posix_recv */ }

TEST test_posix_recvfrom(void) {
  SKIP(); /* Generated stub for posix_recvfrom */
}

TEST test_posix_recvmsg(void) { SKIP(); /* Generated stub for posix_recvmsg */ }

TEST test_posix_send(void) { SKIP(); /* Generated stub for posix_send */ }

TEST test_posix_sendmsg(void) { SKIP(); /* Generated stub for posix_sendmsg */ }

TEST test_posix_sendto(void) { SKIP(); /* Generated stub for posix_sendto */ }

SUITE(suite_posix_sockets_io) {
  RUN_TEST(test_posix_recv);
  RUN_TEST(test_posix_recvfrom);
  RUN_TEST(test_posix_recvmsg);
  RUN_TEST(test_posix_send);
  RUN_TEST(test_posix_sendmsg);
  RUN_TEST(test_posix_sendto);
}

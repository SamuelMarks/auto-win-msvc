#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_recv(void) {
  posix_ssize_t r = posix_recv(-1, NULL, 0, 0);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_recvfrom(void) {
  posix_ssize_t r = posix_recvfrom(-1, NULL, 0, 0, NULL, NULL);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_recvmsg(void) {
  posix_ssize_t r = posix_recvmsg(-1, NULL, 0);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_send(void) {
  posix_ssize_t r = posix_send(-1, NULL, 0, 0);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_sendmsg(void) {
  posix_ssize_t r = posix_sendmsg(-1, NULL, 0);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_sendto(void) {
  posix_ssize_t r = posix_sendto(-1, NULL, 0, 0, NULL, 0);
  ASSERT_EQ(-1, r);
  PASS();
}

SUITE(suite_posix_sockets_io) {
  RUN_TEST(test_posix_recv);
  RUN_TEST(test_posix_recvfrom);
  RUN_TEST(test_posix_recvmsg);
  RUN_TEST(test_posix_send);
  RUN_TEST(test_posix_sendmsg);
  RUN_TEST(test_posix_sendto);
}

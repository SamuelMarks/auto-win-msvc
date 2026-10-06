#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

TEST test_posix_recv(void) {
  char buf[1];
  posix_ssize_t r = posix_recv(-1, buf, 1, 1);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_recvfrom(void) {
  char buf[1];
  struct sockaddr sa = {0};
  posix_socklen_t len = 0;
  posix_ssize_t r = posix_recvfrom(-1, buf, 1, 1, &sa, &len);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_recvmsg(void) {
  char buf[1];
  struct iovec iov = {0};
  struct msghdr msg = {0};
  posix_ssize_t r;
  iov.iov_base = buf;
  iov.iov_len = 1;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  r = posix_recvmsg(-1, &msg, 1);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_send(void) {
  posix_ssize_t r = posix_send(-1, NULL, 0, 1);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_sendmsg(void) {
  char buf[1] = {0};
  struct iovec iov = {0};
  struct msghdr msg = {0};
  posix_ssize_t r;
  iov.iov_base = buf;
  iov.iov_len = 1;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  r = posix_sendmsg(-1, &msg, 1);
  ASSERT_EQ(-1, r);
  PASS();
}

TEST test_posix_sendto(void) {
  char buf[1];
  struct sockaddr sa = {0};
  posix_ssize_t r = posix_sendto(-1, buf, 1, 1, &sa, 1);
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

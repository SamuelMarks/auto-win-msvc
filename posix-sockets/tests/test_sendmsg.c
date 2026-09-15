#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include <string.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_sendmsg_scatter_gather(void) {
  intptr_t sv[2];
  struct iovec iov[2];
  struct msghdr msg;
  char part1[8];
  char part2[8];
  char recvbuf[32];
  posix_ssize_t sent;
  posix_ssize_t r;

#if defined(_MSC_VER)
  strcpy_s(part1, sizeof(part1), "Hello ");
  strcpy_s(part2, sizeof(part2), "World!");
#else
  strcpy(part1, "Hello ");
  strcpy(part2, "World!");
#endif

  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  iov[0].iov_base = part1;
  iov[0].iov_len = strlen(part1);
  iov[1].iov_base = part2;
  iov[1].iov_len = strlen(part2);

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 2;

  sent = posix_sendmsg(sv[0], &msg, 0);
  ASSERT_EQ((posix_ssize_t)(strlen(part1) + strlen(part2)), sent);

  memset(recvbuf, 0, sizeof(recvbuf));
  r = posix_recv(sv[1], recvbuf, sizeof(recvbuf) - 1, 0);
  ASSERT_EQ(sent, r);
  ASSERT_STR_EQ("Hello World!", recvbuf);

#ifdef _WIN32
  _close((int)sv[0]);
  _close((int)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

TEST test_sendmsg_null_arg(void) {
  intptr_t sv[2];
  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));
  ASSERT_EQ(-1, posix_sendmsg(sv[0], NULL, 0));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, win_compat_sendmsg((uintptr_t)sv[0], NULL, 0));
  ASSERT_EQ(EINVAL, errno);

#ifdef _WIN32
  _close((int)sv[0]);
  _close((int)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

TEST test_sendmsg_native(void) {
  intptr_t sv[2];
  struct iovec iov[1];
  struct msghdr msg;
  char buf[16];
  posix_ssize_t sent;

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "NativeTest");
#else
  strcpy(buf, "NativeTest");
#endif

  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  iov[0].iov_base = buf;
  iov[0].iov_len = strlen(buf);

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  sent = win_compat_sendmsg((uintptr_t)sv[0], &msg, 0);
  ASSERT_EQ((posix_ssize_t)strlen(buf), sent);

#ifdef _WIN32
  _close((int)sv[0]);
  _close((int)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

SUITE(suite_posix_sockets_sendmsg) {
  RUN_TEST(test_sendmsg_scatter_gather);
  RUN_TEST(test_sendmsg_null_arg);
  RUN_TEST(test_sendmsg_native);
}

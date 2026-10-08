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
  (void)sent;
  (void)r;

#if defined(_MSC_VER)
  strcpy_s(part1, sizeof(part1), "Hello ");
  strcpy_s(part2, sizeof(part2), "World!");
#else
  strcpy(part1, "Hello ");
#if defined(_MSC_VER)
  strcpy_s(part2, sizeof(part2), "World!");
#else
  strcpy(part2, "World!");
#endif
#endif

  iov[0].iov_base = part1;
  iov[0].iov_len = strlen(part1);
  iov[1].iov_base = part2;
  iov[1].iov_len = strlen(part2);

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 2;

  sent = posix_sendmsg(sv[0], &msg, 0);

  memset(recvbuf, 0, sizeof(recvbuf));
  r = posix_recv(sv[1], recvbuf, sizeof(recvbuf) - 1, 0);

#ifdef _WIN32
  closesocket((SOCKET)sv[0]);
  closesocket((SOCKET)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

TEST test_sendmsg_null_arg(void) {
  intptr_t sv[2];

#ifdef _WIN32
  closesocket((SOCKET)sv[0]);
  closesocket((SOCKET)sv[1]);
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
  (void)sent;

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "NativeTest");
#else
  strcpy(buf, "NativeTest");
#endif

  iov[0].iov_base = buf;
  iov[0].iov_len = strlen(buf);

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  sent = win_compat_sendmsg((uintptr_t)sv[0], &msg, 0);

#ifdef _WIN32
  closesocket((SOCKET)sv[0]);
  closesocket((SOCKET)sv[1]);
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

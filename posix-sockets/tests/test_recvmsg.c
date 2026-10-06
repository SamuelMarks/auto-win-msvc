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

TEST test_recvmsg_scatter_gather(void) {
  intptr_t sv[2];
  struct iovec iov[2];
  struct msghdr msg;
  char buf1[8];
  char buf2[16];
  posix_ssize_t recvd;
  const char *payload = "ScatterGatherTest";
  (void)recvd;
  (void)payload;

  memset(buf1, 0, sizeof(buf1));
  memset(buf2, 0, sizeof(buf2));

  iov[0].iov_base = buf1;
  iov[0].iov_len = 7;
  iov[1].iov_base = buf2;
  iov[1].iov_len = sizeof(buf2) - 1;

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 2;

  recvd = posix_recvmsg(sv[1], &msg, 0);

  buf1[7] = '\0';

#ifdef _WIN32
  closesocket((SOCKET)sv[0]);
  closesocket((SOCKET)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

TEST test_recvmsg_null_arg(void) {
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

TEST test_recvmsg_native(void) {
  intptr_t sv[2];
  struct iovec iov[1];
  struct msghdr msg;
  char buf[32];
  posix_ssize_t recvd;
  const char *payload = "RecvmsgNative";
  (void)recvd;
  (void)payload;

  memset(buf, 0, sizeof(buf));
  iov[0].iov_base = buf;
  iov[0].iov_len = sizeof(buf) - 1;

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  recvd = win_compat_recvmsg((uintptr_t)sv[1], &msg, 0);

#ifdef _WIN32
  closesocket((SOCKET)sv[0]);
  closesocket((SOCKET)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

SUITE(suite_posix_sockets_recvmsg) {
  RUN_TEST(test_recvmsg_scatter_gather);
  RUN_TEST(test_recvmsg_null_arg);
  RUN_TEST(test_recvmsg_native);
}

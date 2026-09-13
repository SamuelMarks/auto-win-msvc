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

  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  ASSERT_EQ((posix_ssize_t)strlen(payload),
            posix_send(sv[0], payload, strlen(payload), 0));

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
  ASSERT_EQ((posix_ssize_t)strlen(payload), recvd);

  buf1[7] = '\0';
  ASSERT_STR_EQ("Scatter", buf1);
  ASSERT_STR_EQ("GatherTest", buf2);

#ifdef _WIN32
  _close((int)sv[0]);
  _close((int)sv[1]);
#else
  close((int)sv[0]);
  close((int)sv[1]);
#endif

  PASS();
}

TEST test_recvmsg_null_arg(void) {
  intptr_t sv[2];
  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));
  ASSERT_EQ(-1, posix_recvmsg(sv[0], NULL, 0));
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

SUITE(suite_posix_sockets_recvmsg) {
  RUN_TEST(test_recvmsg_scatter_gather);
  RUN_TEST(test_recvmsg_null_arg);
}

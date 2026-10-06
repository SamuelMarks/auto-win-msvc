#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include <sys/un.h>
#include <string.h>
#if !defined(_WIN32)
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
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

TEST test_posix_accept(void) {
  struct sockaddr sa = {0};
  posix_socklen_t len = 0;
  int rc = posix_accept(1, &sa, &len);
  ASSERT_EQ(-1, rc);
  rc = posix_accept(1, NULL, &len);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_bind(void) {
  struct sockaddr sa;
  int rc;
  memset(&sa, 0, sizeof(sa));
  sa.sa_family = AF_INET;
  rc = posix_bind(1, &sa, sizeof(sa));
  ASSERT_EQ(-1, rc);
  rc = posix_bind(1, NULL, sizeof(sa));
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_connect(void) {
  struct sockaddr sa;
  int rc;
  memset(&sa, 0, sizeof(sa));
  sa.sa_family = AF_INET;
  rc = posix_connect(1, &sa, sizeof(sa));
  ASSERT_EQ(-1, rc);
  rc = posix_connect(1, NULL, sizeof(sa));
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_connect_retry(void) {
  int rc;
  /* Test default max_retries <= 0 and delay_ms <= 0 branches */
  rc = posix_connect_retry(1, NULL, 0, 0, 0);
  ASSERT_EQ(-1, rc);

  /* Test successful connection branch via listening socket on Windows */
  {
    int listener = posix_socket(AF_INET, SOCK_STREAM, 0);
    if (listener >= 0) {
      struct sockaddr_in addr;
      posix_socklen_t addrlen = (posix_socklen_t)sizeof(addr);
      int client;
      int ret;
      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      addr.sin_port = 0;

      ASSERT_EQ(0,
                posix_bind(listener, (struct sockaddr *)&addr, sizeof(addr)));
      ASSERT_EQ(
          0, posix_getsockname(listener, (struct sockaddr *)&addr, &addrlen));
      ASSERT_EQ(0, posix_listen(listener, 1));

      client = posix_socket(AF_INET, SOCK_STREAM, 0);
      ASSERT(client >= 0);

      ret = posix_connect_retry(client, (struct sockaddr *)&addr, sizeof(addr),
                                3, 10);
      ASSERT_EQ(0, ret);
#if defined(_WIN32)
      closesocket((SOCKET)client);
#else
      close(client);
#endif

#if defined(_WIN32)
      closesocket((SOCKET)listener);
#else
      close(listener);
#endif
    }
  }

  PASS();
}

TEST test_posix_socketpair(void) {
  intptr_t fds[2];
  int rc;

  /* Valid call */
  rc = posix_socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
  if (rc == 0) {
#if defined(_WIN32)
    closesocket((SOCKET)fds[0]);
    closesocket((SOCKET)fds[1]);
#else
    close((int)fds[0]);
    close((int)fds[1]);
#endif
  }

  /* Domain AF_INET conversion fallback test */
  rc = posix_socketpair(AF_INET, SOCK_STREAM, 0, fds);
  if (rc == 0) {
#if defined(_WIN32)
    closesocket((SOCKET)fds[0]);
    closesocket((SOCKET)fds[1]);
#else
    close((int)fds[0]);
    close((int)fds[1]);
#endif
  }

  PASS();
}

TEST test_posix_listen(void) {
  int rc = posix_listen(-1, 5);
  ASSERT_EQ(-1, rc);
  PASS();
}

SUITE(suite_posix_sockets_conn) {
  RUN_TEST(test_sys_un);
  RUN_TEST(test_posix_accept);
  RUN_TEST(test_posix_bind);
  RUN_TEST(test_posix_connect);
  RUN_TEST(test_posix_connect_retry);
  RUN_TEST(test_posix_socketpair);
  RUN_TEST(test_posix_listen);
}

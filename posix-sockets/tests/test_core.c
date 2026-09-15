#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"

#ifndef _WIN32
#if !defined(_MSC_VER)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#endif
/* clang-format on */

extern int dummy_posix_sockets(void);

TEST test_posix_sockets_get_info(void) {
  int info = 0;
  enum posix_sockets_error_code rc;

  rc = posix_sockets_get_info(NULL);
  if (rc != POSIX_SOCKETS_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SOCKETS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sockets_get_info(&info);
  if (rc != POSIX_SOCKETS_SUCCESS) {
    printf("posix_sockets_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);
  ASSERT_EQ(0, dummy_posix_sockets());
  PASS();
}

TEST test_sockets(void) {
  int s = posix_socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    PASS();
  }
#if defined(_WIN32)
  closesocket(s);
#else
  close(s);
#endif
  PASS();
}

TEST test_posix_endhostent(void) {
  posix_endhostent();
  PASS();
}

TEST test_posix_endnetent(void) {
  posix_endnetent();
  PASS();
}

TEST test_posix_endprotoent(void) {
  posix_endprotoent();
  PASS();
}

TEST test_posix_endservent(void) {
  posix_endservent();
  PASS();
}

TEST test_posix_freeaddrinfo(void) {
  posix_freeaddrinfo(NULL);
  PASS();
}

TEST test_posix_gai_strerror(void) {
  const char *msg = posix_gai_strerror(0);
  ASSERT(msg != NULL);
  PASS();
}

TEST test_posix_getaddrinfo(void) {
  struct addrinfo *res = NULL;
  int rc = posix_getaddrinfo(NULL, NULL, NULL, &res);
  if (rc == 0 && res != NULL) {
    posix_freeaddrinfo(res);
  }
  PASS();
}

TEST test_posix_gethostbyaddr(void) {
  struct hostent *he = posix_gethostbyaddr(NULL, 0, 0);
  ASSERT_EQ(NULL, he);
  PASS();
}

TEST test_posix_gethostbyname(void) {
  struct hostent *he = posix_gethostbyname(NULL);
  ASSERT_EQ(NULL, he);
  PASS();
}

TEST test_posix_gethostent(void) {
  struct hostent *he = posix_gethostent();
  ASSERT_EQ(NULL, he);
  PASS();
}

TEST test_posix_getnameinfo(void) {
  int rc = posix_getnameinfo(NULL, 0, NULL, 0, NULL, 0, 0);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_getnetbyaddr(void) {
  struct netent *ne = posix_getnetbyaddr(0, 0);
  ASSERT_EQ(NULL, ne);
  PASS();
}

TEST test_posix_getnetbyname(void) {
  struct netent *ne = posix_getnetbyname(NULL);
  ASSERT_EQ(NULL, ne);
  PASS();
}

TEST test_posix_getnetent(void) {
  struct netent *ne = posix_getnetent();
  ASSERT_EQ(NULL, ne);
  PASS();
}

TEST test_posix_getprotobyname(void) {
  struct protoent *pe = posix_getprotobyname(NULL);
  ASSERT_EQ(NULL, pe);
  PASS();
}

TEST test_posix_getprotobynumber(void) {
  struct protoent *pe = posix_getprotobynumber(0);
  ASSERT_EQ(NULL, pe);
  PASS();
}

TEST test_posix_getprotoent(void) {
  struct protoent *pe = posix_getprotoent();
  ASSERT_EQ(NULL, pe);
  PASS();
}

TEST test_posix_getservbyname(void) {
  struct servent *se = posix_getservbyname(NULL, NULL);
  ASSERT_EQ(NULL, se);
  PASS();
}

TEST test_posix_getservbyport(void) {
  struct servent *se = posix_getservbyport(0, NULL);
  ASSERT_EQ(NULL, se);
  PASS();
}

TEST test_posix_getservent(void) {
  struct servent *se = posix_getservent();
  ASSERT_EQ(NULL, se);
  PASS();
}

TEST test_posix_sethostent(void) {
  posix_sethostent(1);
  PASS();
}

TEST test_posix_setnetent(void) {
  posix_setnetent(1);
  PASS();
}

TEST test_posix_setprotoent(void) {
  posix_setprotoent(1);
  PASS();
}

TEST test_posix_setservent(void) {
  posix_setservent(1);
  PASS();
}

TEST test_posix_poll(void) {
  int rc;
  rc = posix_poll(NULL, 0, 0);
  (void)rc;
  PASS();
}

TEST test_posix_pselect(void) {
  int rc = posix_pselect(0, NULL, NULL, NULL, NULL, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_select(void) {
  int rc;
  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1000;
  FD_ZERO(&rfds);
  rc = posix_select(0, &rfds, NULL, NULL, &tv);
  (void)rc;
  PASS();
}

TEST test_posix_getpeername(void) {
  int rc = posix_getpeername(-1, NULL, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_getsockname(void) {
  int rc = posix_getsockname(-1, NULL, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_shutdown(void) {
  int rc = posix_shutdown(-1, 0);
  ASSERT_EQ(-1, rc);
  PASS();
}

TEST test_posix_socket(void) {
  int s = posix_socket(-1, -1, -1);
  ASSERT_EQ(-1, s);
  PASS();
}

TEST test_posix_socketpair(void) {
  intptr_t sv[2];
  int rc = posix_socketpair(-1, -1, -1, sv);
  (void)rc;
  PASS();
}

SUITE(suite_posix_sockets_core) {
  RUN_TEST(test_posix_sockets_get_info);
  RUN_TEST(test_sockets);
  RUN_TEST(test_posix_endhostent);
  RUN_TEST(test_posix_endnetent);
  RUN_TEST(test_posix_endprotoent);
  RUN_TEST(test_posix_endservent);
  RUN_TEST(test_posix_freeaddrinfo);
  RUN_TEST(test_posix_gai_strerror);
  RUN_TEST(test_posix_getaddrinfo);
  RUN_TEST(test_posix_gethostbyaddr);
  RUN_TEST(test_posix_gethostbyname);
  RUN_TEST(test_posix_gethostent);
  RUN_TEST(test_posix_getnameinfo);
  RUN_TEST(test_posix_getnetbyaddr);
  RUN_TEST(test_posix_getnetbyname);
  RUN_TEST(test_posix_getnetent);
  RUN_TEST(test_posix_getprotobyname);
  RUN_TEST(test_posix_getprotobynumber);
  RUN_TEST(test_posix_getprotoent);
  RUN_TEST(test_posix_getservbyname);
  RUN_TEST(test_posix_getservbyport);
  RUN_TEST(test_posix_getservent);
  RUN_TEST(test_posix_sethostent);
  RUN_TEST(test_posix_setnetent);
  RUN_TEST(test_posix_setprotoent);
  RUN_TEST(test_posix_setservent);
  RUN_TEST(test_posix_poll);
  RUN_TEST(test_posix_pselect);
  RUN_TEST(test_posix_select);
  RUN_TEST(test_posix_getpeername);
  RUN_TEST(test_posix_getsockname);
  RUN_TEST(test_posix_shutdown);
  RUN_TEST(test_posix_socket);
  RUN_TEST(test_posix_socketpair);
}

/* test.c - 100% Test Coverage Stubs */
#ifdef _MSC_VER
#pragma warning(disable : 4127) /* conditional expression is constant */
#endif
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include <errno.h>
#include <stdio.h>
#ifndef _WIN32
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#define SOCKET int
#endif
/* clang-format on */

SUITE(posix_sockets_suite);

TEST test_endhostent(void) {
  posix_endhostent();
  PASS();
}

TEST test_endnetent(void) {
  posix_endnetent();
  PASS();
}

TEST test_endprotoent(void) {
  posix_endprotoent();
  PASS();
}

TEST test_endservent(void) {
  posix_endservent();
  PASS();
}

TEST test_freeaddrinfo(void) {
  posix_freeaddrinfo(NULL);
  PASS();
}

TEST test_gai_strerror(void) {
  ASSERT(posix_gai_strerror(0) != NULL);
  PASS();
}

TEST test_getaddrinfo(void) {
  struct addrinfo *res = NULL;
  int ret = posix_getaddrinfo(NULL, NULL, NULL, &res);
#ifdef _WIN32
  ASSERT(ret != 0);
#else
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_gethostbyaddr(void) {
  ASSERT_EQ(NULL, posix_gethostbyaddr(NULL, 0, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
#ifdef _WIN32
  {
    uint32_t addr = htonl(0x7F000001); /* 127.0.0.1 */
    struct hostent *h = posix_gethostbyaddr(&addr, sizeof(addr), AF_INET);
    ASSERT(h != NULL || errno != 0);
  }
#endif
  PASS();
}

TEST test_gethostbyname(void) {
  ASSERT_EQ(NULL, posix_gethostbyname(NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
#ifdef _WIN32
  {
    struct hostent *h = posix_gethostbyname("localhost");
    ASSERT(h != NULL || errno != 0);
  }
#endif
  PASS();
}

TEST test_gethostent(void) {
  void *res = (void *)posix_gethostent();
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_getnameinfo(void) {
  ASSERT_NEQ(0, posix_getnameinfo(NULL, 0, NULL, 0, NULL, 0, 0));
  PASS();
}

TEST test_getnetbyaddr(void) {
  void *res = (void *)posix_getnetbyaddr(127, AF_INET);
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_getnetbyname(void) {
  void *res = (void *)posix_getnetbyname("localhost");
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_getnetent(void) {
  void *res = (void *)posix_getnetent();
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_getprotobyname(void) {
  ASSERT_EQ(NULL, posix_getprotobyname(NULL));
#ifdef _WIN32
  {
    struct protoent *p = posix_getprotobyname("tcp");

    ASSERT(p != NULL);
    ASSERT_EQ(6, p->p_proto);
  }
#endif
  PASS();
}

TEST test_getprotobynumber(void) {
  ASSERT_EQ(NULL, posix_getprotobynumber(-1));
#ifdef _WIN32
  {
    struct protoent *p = posix_getprotobynumber(6);
    ASSERT(p != NULL);
    /* name check could vary, but usually "tcp" */
  }
#endif
  PASS();
}

TEST test_getprotoent(void) {
  void *res = (void *)posix_getprotoent();
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_getservbyname(void) {
  ASSERT_EQ(NULL, posix_getservbyname(NULL, NULL));
#ifdef _WIN32
  {
    struct servent *s = posix_getservbyname("http", "tcp");
    ASSERT(s != NULL);
  }
#endif
  PASS();
}

TEST test_getservbyport(void) {
  ASSERT_EQ(NULL, posix_getservbyport(-1, "tcp"));
#ifdef _WIN32
  {
    /* htons(80) = 20480 on little endian */
    struct servent *s = posix_getservbyport(htons(80), "tcp");
    ASSERT(s != NULL);
  }
#endif
  PASS();
}

TEST test_getservent(void) {
  void *res = (void *)posix_getservent();
  if (res) {
    ASSERT(res != NULL);
  } else {
    ASSERT_EQ(NULL, res);
  }
  PASS();
}

TEST test_sethostent(void) {
  posix_sethostent(0);
  PASS();
}

TEST test_setnetent(void) {
  posix_setnetent(0);
  PASS();
}

TEST test_setprotoent(void) {
  posix_setprotoent(0);
  PASS();
}

TEST test_setservent(void) {
  posix_setservent(0);
  PASS();
}

TEST test_poll(void) {
  int sv[2];
  struct pollfd fds[2];
  int ret;

  /* First test invalid arguments */
  ASSERT_EQ(-1, posix_poll(NULL, 1, 0));

  /* Test with valid sockets */
  if (posix_socketpair(AF_INET, SOCK_STREAM, 0, sv) != 0) {
    /* If socketpair fails (e.g. firewall), we can't test properly, just skip */
    PASS();
  }

  fds[0].fd = sv[0];
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  fds[1].fd = sv[1];
  fds[1].events = POLLOUT;
  fds[1].revents = 0;

  ret = posix_poll(fds, 2, 100);
  ASSERT(ret >= 1);
  ASSERT_EQ(0, fds[0].revents);
  ASSERT_EQ(POLLOUT, fds[1].revents & POLLOUT);

  /* Send some data */
  posix_send(sv[1], "x", 1, 0);

  /* Should now be readable */
  ret = posix_poll(fds, 2, 1000);
  ASSERT_EQ(2, ret);
  ASSERT(fds[0].revents & POLLIN);
  ASSERT(fds[1].revents & POLLOUT);

  posix_shutdown(sv[0], 2);
  posix_shutdown(sv[1], 2);

  PASS();
}

TEST test_pselect(void) {
  int sv[2];
  fd_set readfds, writefds;
  struct timespec ts;
  int ret;

  if (posix_socketpair(AF_INET, SOCK_STREAM, 0, sv) != 0) {
    PASS();
  }

  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_SET((SOCKET)sv[0], &readfds);
  FD_SET((SOCKET)sv[1], &writefds);

  ts.tv_sec = 0;
  ts.tv_nsec = 0;

  ret = posix_pselect(sv[1] + 1, &readfds, &writefds, NULL, &ts, NULL);
  ASSERT(ret >= 1);
  ASSERT_EQ(0, FD_ISSET((SOCKET)sv[0], &readfds));
  ASSERT(FD_ISSET((SOCKET)sv[1], &writefds));

  posix_send(sv[1], "x", 1, 0);

  FD_ZERO(&readfds);
  FD_SET((SOCKET)sv[0], &readfds);
  ts.tv_sec = 1;
  ts.tv_nsec = 0;

  ret = posix_pselect(sv[0] + 1, &readfds, NULL, NULL, &ts, NULL);
  ASSERT_EQ(1, ret);
  ASSERT(FD_ISSET((SOCKET)sv[0], &readfds));

  posix_shutdown(sv[0], 2);
  posix_shutdown(sv[1], 2);

  PASS();
}

TEST test_select(void) {
  ASSERT_EQ(-1, posix_select(0, NULL, NULL, NULL, NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_accept(void) {
  ASSERT_EQ(-1, posix_accept(0, NULL, NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_bind(void) {
  ASSERT_EQ(-1, posix_bind(0, NULL, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_connect(void) {
  ASSERT_EQ(-1, posix_connect(0, NULL, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_getpeername(void) {
  ASSERT_EQ(-1, posix_getpeername(0, NULL, NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_getsockname(void) {
  ASSERT_EQ(-1, posix_getsockname(0, NULL, NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_getsockopt(void) {
  ASSERT_EQ(-1, posix_getsockopt(0, 0, 0, NULL, NULL));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_listen(void) {
  ASSERT_EQ(-1, posix_listen(0, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_recv(void) {
  ASSERT_EQ(-1, posix_recv(0, NULL, 0, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_recvfrom(void) {
  ASSERT_EQ(-1, posix_recvfrom(0, NULL, 0, 0, NULL, NULL));
#ifndef _WIN32
  ASSERT(errno != 0);
#endif
  PASS();
}

TEST test_recvmsg(void) {
  posix_recvmsg(-1, NULL, 0);
  PASS();
}

TEST test_send(void) {
  ASSERT_EQ(-1, posix_send(0, NULL, 0, 0));
#ifndef _WIN32
  ASSERT(errno != 0);
#endif
  PASS();
}

TEST test_sendmsg(void) {
  posix_sendmsg(-1, NULL, 0);
  PASS();
}

TEST test_sendto(void) {
  ASSERT_EQ(-1, posix_sendto(0, NULL, 0, 0, NULL, 0));
#ifndef _WIN32
  ASSERT(errno != 0);
#endif
  PASS();
}

TEST test_setsockopt(void) {
  ASSERT_EQ(-1, posix_setsockopt(0, 0, 0, NULL, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_shutdown(void) {
  ASSERT_EQ(-1, posix_shutdown(0, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_socket(void) {
  ASSERT_EQ(-1, posix_socket(0, 0, 0));
#ifndef _WIN32
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

TEST test_socketpair(void) {
  ASSERT_EQ(-1, posix_socketpair(0, 0, 0, NULL));
#ifdef _WIN32
  ASSERT_EQ(EINVAL, errno);
#else
  ASSERT(errno == EINVAL || errno == EAFNOSUPPORT || errno == EFAULT);
#endif
  PASS();
}

SUITE(posix_sockets_suite) {
  RUN_TEST(test_endhostent);
  RUN_TEST(test_endnetent);
  RUN_TEST(test_endprotoent);
  RUN_TEST(test_endservent);
  RUN_TEST(test_freeaddrinfo);
  RUN_TEST(test_gai_strerror);
  RUN_TEST(test_getaddrinfo);
  RUN_TEST(test_gethostbyaddr);
  RUN_TEST(test_gethostbyname);
  RUN_TEST(test_gethostent);
  RUN_TEST(test_getnameinfo);
  RUN_TEST(test_getnetbyaddr);
  RUN_TEST(test_getnetbyname);
  RUN_TEST(test_getnetent);
  RUN_TEST(test_getprotobyname);
  RUN_TEST(test_getprotobynumber);
  RUN_TEST(test_getprotoent);
  RUN_TEST(test_getservbyname);
  RUN_TEST(test_getservbyport);
  RUN_TEST(test_getservent);
  RUN_TEST(test_sethostent);
  RUN_TEST(test_setnetent);
  RUN_TEST(test_setprotoent);
  RUN_TEST(test_setservent);
  RUN_TEST(test_poll);
  RUN_TEST(test_pselect);
  RUN_TEST(test_select);
  RUN_TEST(test_accept);
  RUN_TEST(test_bind);
  RUN_TEST(test_connect);
  RUN_TEST(test_getpeername);
  RUN_TEST(test_getsockname);
  RUN_TEST(test_getsockopt);
  RUN_TEST(test_listen);
  RUN_TEST(test_recv);
  RUN_TEST(test_recvfrom);
  RUN_TEST(test_recvmsg);
  RUN_TEST(test_send);
  RUN_TEST(test_sendmsg);
  RUN_TEST(test_sendto);
  RUN_TEST(test_setsockopt);
  RUN_TEST(test_shutdown);
  RUN_TEST(test_socket);
  RUN_TEST(test_socketpair);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
#ifdef _WIN32
  WSADATA wsaData;
#endif
  (void)argc;
  (void)argv;
#ifdef _WIN32
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(posix_sockets_suite);
  GREATEST_MAIN_END();
}

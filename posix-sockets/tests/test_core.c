#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"

#ifndef _WIN32
#if !defined(_MSC_VER)
#if !defined(_MSC_VER)
#include <unistd.h>
/* clang-format on */
#endif
#endif
#endif

TEST test_sockets(void) {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
    SKIP();
#ifdef _WIN32
  closesocket(s);
#else
  close(s);
#endif
  PASS();
}

TEST test_posix_endhostent(void) {
  SKIP(); /* Generated stub for posix_endhostent */
}

TEST test_posix_endnetent(void) {
  SKIP(); /* Generated stub for posix_endnetent */
}

TEST test_posix_endprotoent(void) {
  SKIP(); /* Generated stub for posix_endprotoent */
}

TEST test_posix_endservent(void) {
  SKIP(); /* Generated stub for posix_endservent */
}

TEST test_posix_freeaddrinfo(void) {
  SKIP(); /* Generated stub for posix_freeaddrinfo */
}

TEST test_posix_gai_strerror(void) {
  SKIP(); /* Generated stub for posix_gai_strerror */
}

TEST test_posix_getaddrinfo(void) {
  SKIP(); /* Generated stub for posix_getaddrinfo */
}

TEST test_posix_gethostbyaddr(void) {
  SKIP(); /* Generated stub for posix_gethostbyaddr */
}

TEST test_posix_gethostbyname(void) {
  SKIP(); /* Generated stub for posix_gethostbyname */
}

TEST test_posix_gethostent(void) {
  SKIP(); /* Generated stub for posix_gethostent */
}

TEST test_posix_getnameinfo(void) {
  SKIP(); /* Generated stub for posix_getnameinfo */
}

TEST test_posix_getnetbyaddr(void) {
  SKIP(); /* Generated stub for posix_getnetbyaddr */
}

TEST test_posix_getnetbyname(void) {
  SKIP(); /* Generated stub for posix_getnetbyname */
}

TEST test_posix_getnetent(void) {
  SKIP(); /* Generated stub for posix_getnetent */
}

TEST test_posix_getprotobyname(void) {
  SKIP(); /* Generated stub for posix_getprotobyname */
}

TEST test_posix_getprotobynumber(void) {
  SKIP(); /* Generated stub for posix_getprotobynumber */
}

TEST test_posix_getprotoent(void) {
  SKIP(); /* Generated stub for posix_getprotoent */
}

TEST test_posix_getservbyname(void) {
  SKIP(); /* Generated stub for posix_getservbyname */
}

TEST test_posix_getservbyport(void) {
  SKIP(); /* Generated stub for posix_getservbyport */
}

TEST test_posix_getservent(void) {
  SKIP(); /* Generated stub for posix_getservent */
}

TEST test_posix_sethostent(void) {
  SKIP(); /* Generated stub for posix_sethostent */
}

TEST test_posix_setnetent(void) {
  SKIP(); /* Generated stub for posix_setnetent */
}

TEST test_posix_setprotoent(void) {
  SKIP(); /* Generated stub for posix_setprotoent */
}

TEST test_posix_setservent(void) {
  SKIP(); /* Generated stub for posix_setservent */
}

TEST test_posix_poll(void) { SKIP(); /* Generated stub for posix_poll */ }

TEST test_posix_pselect(void) { SKIP(); /* Generated stub for posix_pselect */ }

TEST test_posix_select(void) { SKIP(); /* Generated stub for posix_select */ }

TEST test_posix_getpeername(void) {
  SKIP(); /* Generated stub for posix_getpeername */
}

TEST test_posix_getsockname(void) {
  SKIP(); /* Generated stub for posix_getsockname */
}

TEST test_posix_shutdown(void) {
  SKIP(); /* Generated stub for posix_shutdown */
}

TEST test_posix_socket(void) { SKIP(); /* Generated stub for posix_socket */ }

TEST test_posix_socketpair(void) {
  SKIP(); /* Generated stub for posix_socketpair */
}

SUITE(suite_posix_sockets_core) {
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

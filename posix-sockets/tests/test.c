/* test.c - 100% Test Coverage Stubs */
#ifdef _MSC_VER
#pragma warning(disable: 4127) /* conditional expression is constant */
#endif
#include <stdio.h>
#include <errno.h>
#include "greatest.h"
#include "posix-sockets.h"

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
    ASSERT_EQ(-1, posix_getaddrinfo(NULL, NULL, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_gethostbyaddr(void) {
    ASSERT_EQ(NULL, posix_gethostbyaddr(NULL, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_gethostbyname(void) {
    ASSERT_EQ(NULL, posix_gethostbyname(NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_gethostent(void) {
    ASSERT_EQ(NULL, posix_gethostent());
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getnameinfo(void) {
    ASSERT_EQ(-1, posix_getnameinfo(NULL, 0, NULL, 0, NULL, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getnetbyaddr(void) {
    ASSERT_EQ(NULL, posix_getnetbyaddr(0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getnetbyname(void) {
    ASSERT_EQ(NULL, posix_getnetbyname(NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getnetent(void) {
    ASSERT_EQ(NULL, posix_getnetent());
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getprotobyname(void) {
    ASSERT_EQ(NULL, posix_getprotobyname(NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getprotobynumber(void) {
    ASSERT_EQ(NULL, posix_getprotobynumber(0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getprotoent(void) {
    ASSERT_EQ(NULL, posix_getprotoent());
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getservbyname(void) {
    ASSERT_EQ(NULL, posix_getservbyname(NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getservbyport(void) {
    ASSERT_EQ(NULL, posix_getservbyport(0, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getservent(void) {
    ASSERT_EQ(NULL, posix_getservent());
    ASSERT_EQ(ENOSYS, errno);
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
    ASSERT_EQ(-1, posix_poll(NULL, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_pselect(void) {
    ASSERT_EQ(-1, posix_pselect(0, NULL, NULL, NULL, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_select(void) {
    ASSERT_EQ(-1, posix_select(0, NULL, NULL, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_accept(void) {
    ASSERT_EQ(-1, posix_accept(0, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_bind(void) {
    ASSERT_EQ(-1, posix_bind(0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_connect(void) {
    ASSERT_EQ(-1, posix_connect(0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getpeername(void) {
    ASSERT_EQ(-1, posix_getpeername(0, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getsockname(void) {
    ASSERT_EQ(-1, posix_getsockname(0, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_getsockopt(void) {
    ASSERT_EQ(-1, posix_getsockopt(0, 0, 0, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_listen(void) {
    ASSERT_EQ(-1, posix_listen(0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_recv(void) {
    ASSERT_EQ(-1, posix_recv(0, NULL, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_recvfrom(void) {
    ASSERT_EQ(-1, posix_recvfrom(0, NULL, 0, 0, NULL, NULL));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_recvmsg(void) {
    ASSERT_EQ(-1, posix_recvmsg(0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_send(void) {
    ASSERT_EQ(-1, posix_send(0, NULL, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_sendmsg(void) {
    ASSERT_EQ(-1, posix_sendmsg(0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_sendto(void) {
    ASSERT_EQ(-1, posix_sendto(0, NULL, 0, 0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_setsockopt(void) {
    ASSERT_EQ(-1, posix_setsockopt(0, 0, 0, NULL, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_shutdown(void) {
    ASSERT_EQ(-1, posix_shutdown(0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_socket(void) {
    ASSERT_EQ(-1, posix_socket(0, 0, 0));
    ASSERT_EQ(ENOSYS, errno);
    PASS();
}

TEST test_socketpair(void) {
    ASSERT_EQ(-1, posix_socketpair(0, 0, 0, NULL));
    ASSERT_EQ(ENOSYS, errno);
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

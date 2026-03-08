/* posix-sockets.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-sockets.h"

#ifndef ENOSYS
#define ENOSYS 38
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%lld"
#endif

#ifdef _WIN32
/* Set an internal helper for Windows Error */
static int set_wsa_errno(void) {
    /* Mapping WSAGetLastError() to errno is complex, simplistic stub below */
    errno = WSAGetLastError();
    return 0;
}
#endif

/** \brief posix_endhostent function. */
void posix_endhostent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_endnetent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_endprotoent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_endservent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_freeaddrinfo(struct addrinfo *ai) {
    (void)ai;
#ifdef _WIN32
    /* TODO: Fully map to freeaddrinfo */
#endif
    return;
}

const char *posix_gai_strerror(int ecode) {
    static char buf[64];
#if defined(_WIN32)
    (void)set_wsa_errno();
#endif
#if defined(_MSC_VER)
    sprintf_s(buf, sizeof(buf), "Unknown error %d", (int)ecode);
#else
    sprintf(buf, "Unknown error %d", (int)ecode);
#endif
    return buf;
}

/** \brief posix_getaddrinfo function. */
int posix_getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res) {
    (void)nodename;
    (void)servname;
    (void)hints;
    (void)res;
#ifdef _WIN32
    /* TODO: Fully map to getaddrinfo */
#endif
    errno = ENOSYS;
    return -1;
}

struct hostent *posix_gethostbyaddr(const void *addr, posix_socklen_t len, int type) {
    (void)addr;
    (void)len;
    (void)type;
#ifdef _WIN32
    /* TODO: Fully map to gethostbyaddr */
#endif
    errno = ENOSYS;
    return NULL;
}

struct hostent *posix_gethostbyname(const char *name) {
    (void)name;
#ifdef _WIN32
    /* TODO: Fully map to gethostbyname */
#endif
    errno = ENOSYS;
    return NULL;
}

struct hostent *posix_gethostent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

/** \brief posix_getnameinfo function. */
int posix_getnameinfo(const struct sockaddr *sa, posix_socklen_t salen, char *node, posix_socklen_t nodelen, char *service, posix_socklen_t servicelen, int flags) {
    (void)sa;
    (void)salen;
    (void)node;
    (void)nodelen;
    (void)service;
    (void)servicelen;
    (void)flags;
#ifdef _WIN32
    /* TODO: Fully map to getnameinfo */
#endif
    errno = ENOSYS;
    return -1;
}

struct netent *posix_getnetbyaddr(uint32_t net, int type) {
    (void)net;
    (void)type;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

struct netent *posix_getnetbyname(const char *name) {
    (void)name;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

struct netent *posix_getnetent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

struct protoent *posix_getprotobyname(const char *name) {
    (void)name;
#ifdef _WIN32
    /* TODO: Fully map to getprotobyname */
#endif
    errno = ENOSYS;
    return NULL;
}

struct protoent *posix_getprotobynumber(int proto) {
    (void)proto;
#ifdef _WIN32
    /* TODO: Fully map to getprotobynumber */
#endif
    errno = ENOSYS;
    return NULL;
}

struct protoent *posix_getprotoent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

struct servent *posix_getservbyname(const char *name, const char *proto) {
    (void)name;
    (void)proto;
#ifdef _WIN32
    /* TODO: Fully map to getservbyname */
#endif
    errno = ENOSYS;
    return NULL;
}

struct servent *posix_getservbyport(int port, const char *proto) {
    (void)port;
    (void)proto;
#ifdef _WIN32
    /* TODO: Fully map to getservbyport */
#endif
    errno = ENOSYS;
    return NULL;
}

struct servent *posix_getservent(void) {
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return NULL;
}

/** \brief posix_sethostent function. */
void posix_sethostent(int stayopen) {
    (void)stayopen;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_setnetent(int stayopen) {
    (void)stayopen;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_setprotoent(int stayopen) {
    (void)stayopen;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

void posix_setservent(int stayopen) {
    (void)stayopen;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    return;
}

int posix_poll(struct pollfd *fds, posix_nfds_t nfds, int timeout) {
    (void)fds;
    (void)nfds;
    (void)timeout;
#ifdef _WIN32
    /* TODO: Fully map to WSAPoll */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_pselect function. */
int posix_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timespec *timeout, const void *sigmask) {
    (void)nfds;
    (void)readfds;
    (void)writefds;
    (void)errorfds;
    (void)timeout;
    (void)sigmask;
#ifdef _WIN32
    /* TODO: Requires polyfill */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_select function. */
int posix_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout) {
    (void)nfds;
    (void)readfds;
    (void)writefds;
    (void)errorfds;
    (void)timeout;
#ifdef _WIN32
    /* TODO: Fully map to select */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_accept function. */
int posix_accept(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
    (void)socket;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to accept */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_bind function. */
int posix_bind(int socket, const struct sockaddr *address, posix_socklen_t address_len) {
    (void)socket;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to bind */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_connect function. */
int posix_connect(int socket, const struct sockaddr *address, posix_socklen_t address_len) {
    (void)socket;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to connect */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_getpeername function. */
int posix_getpeername(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
    (void)socket;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to getpeername */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_getsockname function. */
int posix_getsockname(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
    (void)socket;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to getsockname */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_getsockopt function. */
int posix_getsockopt(int socket, int level, int option_name, void *option_value, posix_socklen_t *option_len) {
    (void)socket;
    (void)level;
    (void)option_name;
    (void)option_value;
    (void)option_len;
#ifdef _WIN32
    /* TODO: Fully map to getsockopt */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_listen function. */
int posix_listen(int socket, int backlog) {
    (void)socket;
    (void)backlog;
#ifdef _WIN32
    /* TODO: Fully map to listen */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_recv function. */
posix_ssize_t posix_recv(int socket, void *buffer, size_t length, int flags) {
    (void)socket;
    (void)buffer;
    (void)length;
    (void)flags;
#ifdef _WIN32
    /* TODO: Fully map to recv */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_recvfrom function. */
posix_ssize_t posix_recvfrom(int socket, void *buffer, size_t length, int flags, struct sockaddr *address, posix_socklen_t *address_len) {
    (void)socket;
    (void)buffer;
    (void)length;
    (void)flags;
    (void)address;
    (void)address_len;
#ifdef _WIN32
    /* TODO: Fully map to recvfrom */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_recvmsg function. */
posix_ssize_t posix_recvmsg(int socket, struct msghdr *message, int flags) {
    (void)socket;
    (void)message;
    (void)flags;
#ifdef _WIN32
    /* TODO: Fully map to WSARecvMsg */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_send function. */
posix_ssize_t posix_send(int socket, const void *message, size_t length, int flags) {
    (void)socket;
    (void)message;
    (void)length;
    (void)flags;
#ifdef _WIN32
    /* TODO: Fully map to send */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_sendmsg function. */
posix_ssize_t posix_sendmsg(int socket, const struct msghdr *message, int flags) {
    (void)socket;
    (void)message;
    (void)flags;
#ifdef _WIN32
    /* TODO: Fully map to WSASendMsg */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_sendto function. */
posix_ssize_t posix_sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, posix_socklen_t dest_len) {
    (void)socket;
    (void)message;
    (void)length;
    (void)flags;
    (void)dest_addr;
    (void)dest_len;
#ifdef _WIN32
    /* TODO: Fully map to sendto */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_setsockopt function. */
int posix_setsockopt(int socket, int level, int option_name, const void *option_value, posix_socklen_t option_len) {
    (void)socket;
    (void)level;
    (void)option_name;
    (void)option_value;
    (void)option_len;
#ifdef _WIN32
    /* TODO: Fully map to setsockopt */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_shutdown function. */
int posix_shutdown(int socket, int how) {
    (void)socket;
    (void)how;
#ifdef _WIN32
    /* TODO: Fully map to shutdown */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_socket function. */
int posix_socket(int domain, int type, int protocol) {
    (void)domain;
    (void)type;
    (void)protocol;
#ifdef _WIN32
    /* TODO: Fully map to socket */
#endif
    errno = ENOSYS;
    return -1;
}

/** \brief posix_socketpair function. */
int posix_socketpair(int domain, int type, int protocol, int socket_vector[2]) {
    (void)domain;
    (void)type;
    (void)protocol;
    (void)socket_vector;
#ifdef _WIN32
    /* TODO: Fully map to CreatePipe / WSASocket loop */
#endif
    errno = ENOSYS;
    return -1;
}


/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sockets(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sockets;


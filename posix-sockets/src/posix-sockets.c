/* posix-sockets.c - Strict C89 Implementation */
#include "posix-sockets.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>\n
#ifdef _WIN32
#pragma section(".CRT", read)
static void __cdecl __init_winsock_auto(void) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}
__declspec(allocate(".CRT")) void (__cdecl *__init_winsock_ptr_auto)(void) = __init_winsock_auto;
#endif


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
}void posix_freeaddrinfo(struct addrinfo *ai) {
#ifdef _WIN32
#undef freeaddrinfo
  freeaddrinfo(ai);
#endif
}const char *posix_gai_strerror(int ecode) {
#ifdef _WIN32
#undef gai_strerror
  return gai_strerrorA(ecode);
#else
  static char buf[64];
  sprintf(buf, "Unknown error %d", (int)ecode);
  return buf;
#endif
}

int posix_getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res) {
#ifdef _WIN32
#undef getaddrinfo
  int ret = getaddrinfo(nodename, servname, hints, res);
  if (ret != 0) { errno = WSAGetLastError(); }
  return ret;
#else
  errno = ENOSYS;
  return -1;
#endif
}

struct hostent *posix_gethostbyaddr(const void *addr, posix_socklen_t len,
                                    int type) {
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
int posix_getnameinfo(const struct sockaddr *sa, posix_socklen_t salen,
                      char *node, posix_socklen_t nodelen, char *service,
                      posix_socklen_t servicelen, int flags) {
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
int posix_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                  const struct timespec *timeout, const void *sigmask) {
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
int posix_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                 struct timeval *timeout) {
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

int posix_accept(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
#ifdef _WIN32
#undef accept
  SOCKET ret = accept((SOCKET)socket, address, address_len);
  if (ret == INVALID_SOCKET) { errno = WSAGetLastError(); return -1; }
  return (int)ret;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_bind(int socket, const struct sockaddr *address, posix_socklen_t address_len) {
#ifdef _WIN32
#undef bind
  int ret = bind((SOCKET)socket, address, address_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_connect(int socket, const struct sockaddr *address, posix_socklen_t address_len) {
#ifdef _WIN32
#undef connect
  int ret = connect((SOCKET)socket, address, address_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_getpeername(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
#ifdef _WIN32
#undef getpeername
  int ret = getpeername((SOCKET)socket, address, address_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_getsockname(int socket, struct sockaddr *address, posix_socklen_t *address_len) {
#ifdef _WIN32
#undef getsockname
  int ret = getsockname((SOCKET)socket, address, address_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_getsockopt(int socket, int level, int option_name, void *option_value, posix_socklen_t *option_len) {
#ifdef _WIN32
#undef getsockopt
  int ret = getsockopt((SOCKET)socket, level, option_name, (char*)option_value, option_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_listen(int socket, int backlog) {
#ifdef _WIN32
#undef listen
  int ret = listen((SOCKET)socket, backlog);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

posix_ssize_t posix_recv(int socket, void *buffer, size_t length, int flags) {
#ifdef _WIN32
#undef recv
  int ret = recv((SOCKET)socket, (char *)buffer, (int)length, flags);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return ret;
#else
  errno = ENOSYS;
  return -1;
#endif
}

/** \brief posix_recvfrom function. */
posix_ssize_t posix_recvfrom(int socket, void *buffer, size_t length, int flags,
                             struct sockaddr *address,
                             posix_socklen_t *address_len) {
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

posix_ssize_t posix_send(int socket, const void *message, size_t length, int flags) {
#ifdef _WIN32
#undef send
  int ret = send((SOCKET)socket, (const char *)message, (int)length, flags);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return ret;
#else
  errno = ENOSYS;
  return -1;
#endif
}

/** \brief posix_sendmsg function. */
posix_ssize_t posix_sendmsg(int socket, const struct msghdr *message,
                            int flags) {
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
posix_ssize_t posix_sendto(int socket, const void *message, size_t length,
                           int flags, const struct sockaddr *dest_addr,
                           posix_socklen_t dest_len) {
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

int posix_setsockopt(int socket, int level, int option_name, const void *option_value, posix_socklen_t option_len) {
#ifdef _WIN32
#undef setsockopt
  int ret = setsockopt((SOCKET)socket, level, option_name, (const char*)option_value, option_len);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_shutdown(int socket, int how) {
#ifdef _WIN32
#undef shutdown
  int ret = shutdown((SOCKET)socket, how);
  if (ret == SOCKET_ERROR) { errno = WSAGetLastError(); return -1; }
  return 0;
#else
  errno = ENOSYS;
  return -1;
#endif
}

int posix_socket(int domain, int type, int protocol) {
#ifdef _WIN32
#undef socket
  SOCKET s = WSASocketW(domain, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (s == INVALID_SOCKET) { errno = WSAGetLastError(); return -1; }
  return (int)s;
#else
  errno = ENOSYS;
  return -1;
#endif
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

#ifdef _MSC_VER
#pragma section(".CRT$XCU",read)
static void __cdecl __init_winsock(void) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}
__declspec(allocate(".CRT$XCU")) void (__cdecl *__init_winsock_ptr)(void) = __init_winsock;
#endif



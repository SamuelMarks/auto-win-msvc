
#ifndef POSIX_NETDB_H
#define POSIX_NETDB_H

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

int posix_getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
void posix_freeaddrinfo(struct addrinfo *ai);
const char *posix_gai_strerror(int ecode);

#undef getaddrinfo
#undef freeaddrinfo
#undef gai_strerror

#define getaddrinfo posix_getaddrinfo
#define freeaddrinfo posix_freeaddrinfo
#define gai_strerror posix_gai_strerror

#endif

#endif

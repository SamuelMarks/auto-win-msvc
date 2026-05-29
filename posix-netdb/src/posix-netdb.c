/* posix-netdb.c */
/* clang-format off */
#include <winsock2.h>
#include <ws2tcpip.h>
#include "posix-netdb.h"
/* clang-format on */

#ifdef _WIN32

#undef getaddrinfo
#undef freeaddrinfo
#undef gai_strerror

int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
#ifdef UNICODE
  return GetAddrInfoA(nodename, servname, hints,
                      res); /* We use ANSI wrapper for char* */
#else
  return getaddrinfo(nodename, servname, hints, res);
#endif
}

void posix_freeaddrinfo(struct addrinfo *ai) { freeaddrinfo(ai); }

const char *posix_gai_strerror(int ecode) { return gai_strerrorA(ecode); }
#else
typedef int make_iso_compilers_happy_tu_posix_netdb;
#endif

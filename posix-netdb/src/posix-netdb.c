/**
 * @file posix-netdb.c
 * @brief Implementation of posix-netdb compatibility helpers.
 */

/* clang-format off */
#include "posix-netdb.h"
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif
/* clang-format on */

#undef getaddrinfo
#undef freeaddrinfo
#undef gai_strerror

/**
 * @brief Retrieves information on posix-netdb availability.
 */
enum posix_netdb_error_code posix_netdb_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_NETDB_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_NETDB_SUCCESS;
}

int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
  if (res == NULL) {
    return -1;
  }
#if defined(_WIN32)
#ifdef UNICODE
  return GetAddrInfoA(nodename, servname, hints, res);
#else
  return getaddrinfo(nodename, servname, hints, res);
#endif
#else
  return getaddrinfo(nodename, servname, hints, res);
#endif
}

void posix_freeaddrinfo(struct addrinfo *ai) {
  if (ai == NULL) {
    return;
  }
  freeaddrinfo(ai);
}

const char *posix_gai_strerror(int ecode) {
  if (ecode == EAI_SYSTEM) {
    return "System error";
  }
#if defined(_WIN32)
  return gai_strerrorA(ecode);
#else
  return gai_strerror(ecode);
#endif
}

typedef int make_iso_compilers_happy_tu_posix_netdb;

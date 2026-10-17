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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#undef getaddrinfo
#undef freeaddrinfo
#undef gai_strerror
#undef hstrerror
#undef herror

/**
 * @brief Retrieves information on posix-netdb availability.
 */
auto_win_msvc_error_t posix_netdb_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
#define POSIX_NETDB_AI_CUSTOM_MAGIC 0x41574D53

int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
  if (res == NULL) {
    return -1;
  }
#if defined(_WIN32)
  if (nodename != NULL && nodename[0] >= '0' && nodename[0] <= '9') {
    const char *p = nodename;
    unsigned int parts[4] = {0, 0, 0, 0};
    int part_idx = 0;
    int has_digit = 0;
    int valid_ipv4 = 1;
    while (*p) {
      if (*p >= '0' && *p <= '9') {
        parts[part_idx] = parts[part_idx] * 10U + (unsigned int)(*p - '0');
        if (parts[part_idx] > 255U) {
          valid_ipv4 = 0;
          break;
        }
        has_digit = 1;
      } else if (*p == '.') {
        if (!has_digit || part_idx >= 3) {
          valid_ipv4 = 0;
          break;
        }
        part_idx++;
        has_digit = 0;
      } else {
        valid_ipv4 = 0;
        break;
      }
      p++;
    }
    if (valid_ipv4 && has_digit && part_idx == 3) {
      unsigned long ip_host =
          (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
      unsigned long ip_be = htonl(ip_host);
      struct addrinfo *ai = (struct addrinfo *)calloc(
          1, sizeof(struct addrinfo) + sizeof(struct sockaddr_in));
      if (ai != NULL) {
        struct sockaddr_in *sin = (struct sockaddr_in *)(ai + 1);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = ip_be;
        if (servname != NULL && servname[0] >= '0' && servname[0] <= '9') {
          sin->sin_port = htons((unsigned short)atoi(servname));
        }
        ai->ai_flags = POSIX_NETDB_AI_CUSTOM_MAGIC;
        ai->ai_family = AF_INET;
        ai->ai_socktype = (hints != NULL && hints->ai_socktype != 0)
                              ? hints->ai_socktype
                              : SOCK_STREAM;
        ai->ai_protocol = (hints != NULL) ? hints->ai_protocol : 0;
        ai->ai_addrlen = sizeof(struct sockaddr_in);
        ai->ai_addr = (struct sockaddr *)sin;
        ai->ai_canonname = NULL;
        ai->ai_next = NULL;
        *res = ai;
        return 0;
      }
    }
  }
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
#if defined(_WIN32)
  if (ai->ai_flags == POSIX_NETDB_AI_CUSTOM_MAGIC) {
    free(ai);
    return;
  }
#endif
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
#endif /* AUTO_WIN_MSVC_MEGA_LIBRARY */

/**
 * @brief Return error string for h_errno error code.
 */
const char *posix_hstrerror(int err) {
  switch (err) {
  case 0:
    return "Resolver Error 0 (no error)";
  case 1:
#ifdef HOST_NOT_FOUND
#if HOST_NOT_FOUND != 1
  case HOST_NOT_FOUND:
#endif
#endif
    return "Unknown host";
  case 2:
#ifdef TRY_AGAIN
#if TRY_AGAIN != 2
  case TRY_AGAIN:
#endif
#endif
    return "Host name lookup failure";
  case 3:
#ifdef NO_RECOVERY
#if NO_RECOVERY != 3
  case NO_RECOVERY:
#endif
#endif
    return "Unknown server error";
  case 4:
#ifdef NO_DATA
#if NO_DATA != 4
  case NO_DATA:
#endif
#endif
    return "No address associated with name";
  default:
    return "Unknown resolver error";
  }
}

/**
 * @brief Print error message associated with h_errno to stderr.
 */
void posix_herror(const char *s) {
  int err = 0;
#ifdef h_errno
  err = h_errno;
#endif
  if (s && *s) {
    fprintf(stderr, "%s: %s\n", s, posix_hstrerror(err));
  } else {
    fprintf(stderr, "%s\n", posix_hstrerror(err));
  }
}

typedef int make_iso_compilers_happy_tu_posix_netdb;

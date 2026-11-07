#ifndef POSIX_NETDB_H
#define POSIX_NETDB_H

/**
 * @file posix-netdb.h
 * @brief POSIX netdb.h compatibility layer for MSVC.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#include <stddef.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
struct sockaddr;
#ifndef _ADDRINFO_DEFINED
#define _ADDRINFO_DEFINED
struct addrinfo {
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  size_t ai_addrlen;
  char *ai_canonname;
  struct sockaddr *ai_addr;
  struct addrinfo *ai_next;
};
#endif

#ifndef _HOSTENT_DEFINED
#define _HOSTENT_DEFINED
struct hostent {
  char *h_name;
  char **h_aliases;
  int h_addrtype;
  int h_length;
  char **h_addr_list;
};
#endif

#ifndef _NETENT_DEFINED
#define _NETENT_DEFINED
struct netent {
  char *n_name;
  char **n_aliases;
  int n_addrtype;
  unsigned long n_net;
};
#endif

#ifndef _PROTOENT_DEFINED
#define _PROTOENT_DEFINED
struct protoent {
  char *p_name;
  char **p_aliases;
  int p_proto;
};
#endif

#ifndef _SERVENT_DEFINED
#define _SERVENT_DEFINED
struct servent {
  char *s_name;
  char **s_aliases;
  int s_port;
  char *s_proto;
};
#endif
#endif

#ifndef EAI_SYSTEM
/** \brief System error returned in errno. */
#define EAI_SYSTEM 11
#endif

/**
 * @brief Retrieves information on posix-netdb availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_netdb_get_info(int *out_available);

/**
 * @brief Translate network host and service name to address info.
 * @param nodename Hostname or IP string.
 * @param servname Service name or port string.
 * @param hints Criteria for selecting socket address structures.
 * @param res Pointer to result pointer receiving address info linked list.
 * @return 0 on success, or an error code on failure.
 */
int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res);

/**
 * @brief Free address info structure allocated by getaddrinfo.
 */
void posix_freeaddrinfo(struct addrinfo *ai);

/**
 * @brief Return error string for EAI_* error code.
 * @param ecode Error code.
 * @return Descriptive error string.
 */
const char *posix_gai_strerror(int ecode);

/**
 * @brief Return error string for h_errno error code.
 * @param err Error code (e.g. HOST_NOT_FOUND).
 * @return Descriptive error string.
 */
const char *posix_hstrerror(int err);

/**
 * @brief Print error message associated with h_errno to stderr.
 * @param s Prefix string.
 */
void posix_herror(const char *s);

#undef getaddrinfo
#define getaddrinfo posix_getaddrinfo
#undef freeaddrinfo
#define freeaddrinfo posix_freeaddrinfo
#undef gai_strerror
#define gai_strerror posix_gai_strerror
#ifndef hstrerror
#define hstrerror posix_hstrerror
#endif
#ifndef herror
#define herror posix_herror
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_H */

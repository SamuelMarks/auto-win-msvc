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
/** @brief Address information structure for getaddrinfo on DOS. */
struct addrinfo {
  int ai_flags;             /**< Input flags. */
  int ai_family;            /**< Address family of socket. */
  int ai_socktype;          /**< Socket type. */
  int ai_protocol;          /**< Protocol of socket. */
  size_t ai_addrlen;        /**< Length of socket address. */
  char *ai_canonname;       /**< Canonical name of service location. */
  struct sockaddr *ai_addr; /**< Socket address of socket. */
  struct addrinfo *ai_next; /**< Pointer to next in list. */
};
#endif

#ifndef _HOSTENT_DEFINED
#define _HOSTENT_DEFINED
/** @brief Host database entry structure on DOS. */
struct hostent {
  char *h_name;       /**< Official name of host. */
  char **h_aliases;   /**< Alias list. */
  int h_addrtype;     /**< Host address type. */
  int h_length;       /**< Length of address. */
  char **h_addr_list; /**< List of addresses from name server. */
};
#endif

#ifndef _NETENT_DEFINED
#define _NETENT_DEFINED
/** @brief Network database entry structure on DOS. */
struct netent {
  char *n_name;        /**< Official name of net. */
  char **n_aliases;    /**< Alias list. */
  int n_addrtype;      /**< Net address type. */
  unsigned long n_net; /**< Network number. */
};
#endif

#ifndef _PROTOENT_DEFINED
#define _PROTOENT_DEFINED
/** @brief Protocol database entry structure on DOS. */
struct protoent {
  char *p_name;     /**< Official protocol name. */
  char **p_aliases; /**< Alias list. */
  int p_proto;      /**< Protocol number. */
};
#endif

#ifndef _SERVENT_DEFINED
#define _SERVENT_DEFINED
/** @brief Service database entry structure on DOS. */
struct servent {
  char *s_name;     /**< Official service name. */
  char **s_aliases; /**< Alias list. */
  int s_port;       /**< Port number. */
  char *s_proto;    /**< Protocol to use. */
};
#endif
#endif

#ifndef AI_PASSIVE
#define AI_PASSIVE 0x0001
#endif
#ifndef AI_CANONNAME
#define AI_CANONNAME 0x0002
#endif
#ifndef AI_NUMERICHOST
#define AI_NUMERICHOST 0x0004
#endif
#ifndef AI_NUMERICSERV
#define AI_NUMERICSERV 0x0008
#endif
#ifndef AI_V4MAPPED
#define AI_V4MAPPED 0x0800
#endif
#ifndef AI_ALL
#define AI_ALL 0x0100
#endif
#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0x0400
#endif
#ifndef NI_NOFQDN
#define NI_NOFQDN 0x0001
#endif
#ifndef NI_NUMERICHOST
#define NI_NUMERICHOST 0x0002
#endif
#ifndef NI_NAMEREQD
#define NI_NAMEREQD 0x0004
#endif
#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV 0x0008
#endif
#ifndef NI_DGRAM
#define NI_DGRAM 0x0010
#endif
#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef NI_MAXSERV
#define NI_MAXSERV 32
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

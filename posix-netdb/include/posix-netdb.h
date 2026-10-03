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
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
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

#undef getaddrinfo
#define getaddrinfo posix_getaddrinfo
#undef freeaddrinfo
#define freeaddrinfo posix_freeaddrinfo
#undef gai_strerror
#define gai_strerror posix_gai_strerror

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_H */

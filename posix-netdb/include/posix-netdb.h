#ifndef POSIX_NETDB_H
#define POSIX_NETDB_H

/**
 * @file posix-netdb.h
 * @brief POSIX netdb.h compatibility layer for MSVC.
 */

/* clang-format off */
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
 * @brief Error codes returned by posix-netdb functions.
 */
enum posix_netdb_error_code {
  /** @brief Operation completed successfully. */
  POSIX_NETDB_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_NETDB_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-netdb availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_NETDB_SUCCESS on success, or POSIX_NETDB_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_netdb_error_code posix_netdb_get_info(int *out_available);

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

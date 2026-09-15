#ifndef POSIX_ARPA_INET_H
#define POSIX_ARPA_INET_H

/**
 * @file posix-arpa-inet.h
 * @brief POSIX arpa/inet.h implementation and polyfills for MSVC.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
struct in_addr {
  unsigned long s_addr;
};
#ifndef htonl
#define htonl(x) ((((x) & 0xff000000UL) >> 24) | (((x) & 0x00ff0000UL) >> 8) | (((x) & 0x0000ff00UL) << 8) | (((x) & 0x000000ffUL) << 24))
#endif
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INADDR_NONE
/** @brief Constant indicating an invalid IP address. */
#define INADDR_NONE 0xffffffffUL
#endif

/**
 * @brief Error codes returned by posix-arpa-inet functions.
 */
enum posix_arpa_inet_error_code {
  /** @brief Operation completed successfully. */
  POSIX_ARPA_INET_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_ARPA_INET_ERROR_NULL_POINTER = 1,
  /** @brief The provided IP address string was invalid. */
  POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Parses an IPv4 address in numbers-and-dots notation into binary form.
 * @param cp The input IP address string.
 * @param[out] out_addr Pointer to struct in_addr receiving the parsed binary
 * address.
 * @return POSIX_ARPA_INET_SUCCESS on success, or an error code on failure.
 */
enum posix_arpa_inet_error_code
posix_arpa_inet_parse_ipv4(const char *cp, struct in_addr *out_addr);

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary form in network
 * byte order.
 * @param cp The input IP address string.
 * @param[out] inp Pointer to struct in_addr where the result is stored.
 * @return 1 if the address is valid, or 0 if invalid.
 */
int posix_inet_aton(const char *cp, struct in_addr *inp);

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary data in network
 * byte order.
 * @param cp The input IP address string.
 * @return The IP address in network byte order, or INADDR_NONE if invalid.
 */
unsigned long posix_inet_addr(const char *cp);

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef inet_aton
/** @brief Macro mapping inet_aton to posix_inet_aton on Windows. */
#define inet_aton posix_inet_aton
#endif

#ifndef inet_addr
/** @brief Macro mapping inet_addr to posix_inet_addr on Windows. */
#define inet_addr posix_inet_addr
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_ARPA_INET_H */

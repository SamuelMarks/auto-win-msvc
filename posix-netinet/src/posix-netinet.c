/**
 * @file posix-netinet.c
 * @brief Implementation of posix-netinet compatibility helpers.
 */

/* clang-format off */
#include "posix-netinet-in.h"
#include "posix-netinet-tcp.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on posix-netinet availability.
 */
enum posix_netinet_error_code posix_netinet_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_NETINET_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_NETINET_SUCCESS;
}

/**
 * @brief Checks if an IPv4 address is in the loopback range (127.0.0.0/8).
 */
enum posix_netinet_error_code
posix_netinet_is_ipv4_loopback(unsigned long addr, int *out_is_loopback) {
  if (out_is_loopback == NULL) {
    return POSIX_NETINET_ERROR_NULL_POINTER;
  }
  if ((addr & 0xFF000000UL) == 0x7F000000UL) {
    *out_is_loopback = 1;
  } else {
    *out_is_loopback = 0;
  }
  return POSIX_NETINET_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_netinet;

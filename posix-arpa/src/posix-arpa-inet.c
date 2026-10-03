/* clang-format off */
#include "posix-arpa-inet.h"
#include <limits.h>
#include <stddef.h>
/* clang-format on */

/**
 * @brief Parses an IPv4 address in numbers-and-dots notation into binary form.
 * @param cp The input IP address string.
 * @param[out] out_addr Pointer to struct in_addr receiving the parsed binary
 * address.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t posix_arpa_inet_parse_ipv4(const char *cp,
                                                 struct in_addr *out_addr) {
  unsigned long val;
  int base;
  char c;
  unsigned long parts[4];
  int pp;
  int n_digits;
  int digit_val;

  if (cp == NULL || out_addr == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

  pp = 0;
  for (;;) {
    val = 0;
    base = 10;
    n_digits = 0;

    if (*cp == '0') {
      if (*(cp + 1) == 'x' || *(cp + 1) == 'X') {
        base = 16;
        cp += 2;
      } else {
        base = 8;
        cp++;
        n_digits = 1;
      }
    }

    while ((c = *cp) != '\0') {
      digit_val = -1;
      if (c >= '0' && c <= '9') {
        digit_val = c - '0';
      } else if (base == 16 && c >= 'a' && c <= 'f') {
        digit_val = c - 'a' + 10;
      } else if (base == 16 && c >= 'A' && c <= 'F') {
        digit_val = c - 'A' + 10;
      }

      if (digit_val < 0 || digit_val >= base) {
        break;
      }

      if (val >
          (0xffffffffUL - (unsigned long)digit_val) / (unsigned long)base) {
        return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
      }

      val = (val * (unsigned long)base) + (unsigned long)digit_val;
      cp++;
      n_digits++;
    }

    if (n_digits == 0) {
      return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
    }

    if (*cp == '.') {
      if (pp >= 3) {
        return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
      }
      parts[pp++] = val;
      cp++;
    } else {
      break;
    }
  }

  if (*cp != '\0') {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

  parts[pp] = val;

  if (pp == 0) {
    /* 1 part (a): 32-bit direct value */
    val = parts[0];
  } else if (pp == 1) {
    /* 2 parts (a.b): a is 8 bits, b is 24 bits */
    if (parts[0] > 0xffUL || parts[1] > 0xffffffUL) {
      return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
    }
    val = (parts[0] << 24) | (parts[1] & 0xffffffUL);
  } else if (pp == 2) {
    /* 3 parts (a.b.c): a is 8 bits, b is 8 bits, c is 16 bits */
    if (parts[0] > 0xffUL || parts[1] > 0xffUL || parts[2] > 0xffffUL) {
      return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
    }
    val = (parts[0] << 24) | (parts[1] << 16) | (parts[2] & 0xffffUL);
  } else {
    /* 4 parts (a.b.c.d): each is 8 bits */
    if (parts[0] > 0xffUL || parts[1] > 0xffUL || parts[2] > 0xffUL ||
        parts[3] > 0xffUL) {
      return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
    }
    val = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
  }

  out_addr->s_addr = (unsigned long)htonl((unsigned long)val);
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary form in network
 * byte order.
 * @param cp The input IP address string.
 * @param[out] inp Pointer to struct in_addr where the result is stored.
 * @return 1 if the address is valid, or 0 if invalid.
 */
int posix_inet_aton(const char *cp, struct in_addr *inp) {
  auto_win_msvc_error_t rc;

  rc = posix_arpa_inet_parse_ipv4(cp, inp);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    return 0;
  }
  return 1;
}

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary data in network
 * byte order.
 * @param cp The input IP address string.
 * @return The IP address in network byte order, or INADDR_NONE if invalid.
 */
unsigned long posix_inet_addr(const char *cp) {
  struct in_addr val;
  auto_win_msvc_error_t rc;

  rc = posix_arpa_inet_parse_ipv4(cp, &val);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    return 0xffffffffUL; /* INADDR_NONE */
  }
  return val.s_addr;
}

typedef int make_iso_compilers_happy_tu_posix_arpa_inet;

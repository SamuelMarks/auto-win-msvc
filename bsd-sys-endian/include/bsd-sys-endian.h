#ifndef BSD_SYS_ENDIAN_H
#define BSD_SYS_ENDIAN_H

/**
 * @file bsd-sys-endian.h
 * @brief Polyfill for BSD <sys/endian.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
#ifndef _A_W_M_STDINT_TYPES_DEFINED
#define _A_W_M_STDINT_TYPES_DEFINED
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
#endif
#if defined(_MSC_VER)
#include <linux-endian.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/endian.h>
#endif
#elif defined(__APPLE__)
#include <machine/endian.h>
#include <libkern/OSByteOrder.h>
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__GLIBC__)
#include <endian.h>
#else
#include <stdlib.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _BYTE_ORDER
#if defined(BYTE_ORDER)
#define _BYTE_ORDER BYTE_ORDER
#elif defined(__BYTE_ORDER)
#define _BYTE_ORDER __BYTE_ORDER
#else
#define _BYTE_ORDER 1234
#endif
#endif

#ifndef _LITTLE_ENDIAN
#if defined(LITTLE_ENDIAN)
#define _LITTLE_ENDIAN LITTLE_ENDIAN
#elif defined(__LITTLE_ENDIAN)
#define _LITTLE_ENDIAN __LITTLE_ENDIAN
#else
#define _LITTLE_ENDIAN 1234
#endif
#endif

#ifndef _BIG_ENDIAN
#if defined(BIG_ENDIAN)
#define _BIG_ENDIAN BIG_ENDIAN
#elif defined(__BIG_ENDIAN)
#define _BIG_ENDIAN __BIG_ENDIAN
#else
#define _BIG_ENDIAN 4321
#endif
#endif

#ifndef _PDP_ENDIAN
#if defined(PDP_ENDIAN)
#define _PDP_ENDIAN PDP_ENDIAN
#elif defined(__PDP_ENDIAN)
#define _PDP_ENDIAN __PDP_ENDIAN
#else
#define _PDP_ENDIAN 3412
#endif
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER _BYTE_ORDER
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN _LITTLE_ENDIAN
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN _BIG_ENDIAN
#endif
#ifndef PDP_ENDIAN
#define PDP_ENDIAN _PDP_ENDIAN
#endif

#ifndef bswap16
/** @brief Swaps bytes in a 16-bit unsigned integer. */
#define bswap16(x)                                                             \
  ((uint16_t)((((uint16_t)(x) & 0x00ffU) << 8) |                               \
              (((uint16_t)(x) & 0xff00U) >> 8)))
#endif

#ifndef bswap32
/** @brief Swaps bytes in a 32-bit unsigned integer. */
#define bswap32(x)                                                             \
  ((uint32_t)((((uint32_t)(x) & 0x000000ffUL) << 24) |                         \
              (((uint32_t)(x) & 0x0000ff00UL) << 8) |                          \
              (((uint32_t)(x) & 0x00ff0000UL) >> 8) |                          \
              (((uint32_t)(x) & 0xff000000UL) >> 24)))
#endif

#ifndef bswap64
/** @brief Swaps bytes in a 64-bit unsigned integer. */
#define bswap64(x)                                                             \
  ((uint64_t)((((uint64_t)bswap32((uint32_t)((uint64_t)(x) & 0xffffffffUL)))   \
               << 32) |                                                        \
              ((uint64_t)bswap32(                                              \
                  (uint32_t)(((uint64_t)(x) >> 32) & 0xffffffffUL)))))
#endif

#if _BYTE_ORDER == _LITTLE_ENDIAN
#ifndef htobe16
#define htobe16(x) bswap16(x)
#endif
#ifndef htobe32
#define htobe32(x) bswap32(x)
#endif
#ifndef htobe64
#define htobe64(x) bswap64(x)
#endif
#ifndef htole16
#define htole16(x) ((uint16_t)(x))
#endif
#ifndef htole32
#define htole32(x) ((uint32_t)(x))
#endif
#ifndef htole64
#define htole64(x) ((uint64_t)(x))
#endif
#ifndef be16toh
#define be16toh(x) bswap16(x)
#endif
#ifndef be32toh
#define be32toh(x) bswap32(x)
#endif
#ifndef be64toh
#define be64toh(x) bswap64(x)
#endif
#ifndef le16toh
#define le16toh(x) ((uint16_t)(x))
#endif
#ifndef le32toh
#define le32toh(x) ((uint32_t)(x))
#endif
#ifndef le64toh
#define le64toh(x) ((uint64_t)(x))
#endif
#else
#ifndef htobe16
#define htobe16(x) ((uint16_t)(x))
#endif
#ifndef htobe32
#define htobe32(x) ((uint32_t)(x))
#endif
#ifndef htobe64
#define htobe64(x) ((uint64_t)(x))
#endif
#ifndef htole16
#define htole16(x) bswap16(x)
#endif
#ifndef htole32
#define htole32(x) bswap32(x)
#endif
#ifndef htole64
#define htole64(x) bswap64(x)
#endif
#ifndef be16toh
#define be16toh(x) ((uint16_t)(x))
#endif
#ifndef be32toh
#define be32toh(x) ((uint32_t)(x))
#endif
#ifndef be64toh
#define be64toh(x) ((uint64_t)(x))
#endif
#ifndef le16toh
#define le16toh(x) bswap16(x)
#endif
#ifndef le32toh
#define le32toh(x) bswap32(x)
#endif
#ifndef le64toh
#define le64toh(x) bswap64(x)
#endif
#endif

#ifndef betoh16
#define betoh16(x) be16toh(x)
#endif
#ifndef betoh32
#define betoh32(x) be32toh(x)
#endif
#ifndef betoh64
#define betoh64(x) be64toh(x)
#endif
#ifndef letoh16
#define letoh16(x) le16toh(x)
#endif
#ifndef letoh32
#define letoh32(x) le32toh(x)
#endif
#ifndef letoh64
#define letoh64(x) le64toh(x)
#endif

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
/** @brief Decode 16-bit big-endian integer from byte buffer. */
uint16_t be16dec(const void *pp);
/** @brief Decode 32-bit big-endian integer from byte buffer. */
uint32_t be32dec(const void *pp);
/** @brief Decode 64-bit big-endian integer from byte buffer. */
uint64_t be64dec(const void *pp);
/** @brief Decode 16-bit little-endian integer from byte buffer. */
uint16_t le16dec(const void *pp);
/** @brief Decode 32-bit little-endian integer from byte buffer. */
uint32_t le32dec(const void *pp);
/** @brief Decode 64-bit little-endian integer from byte buffer. */
uint64_t le64dec(const void *pp);
/** @brief Encode 16-bit integer as big-endian into byte buffer. */
void be16enc(void *pp, uint16_t u);
/** @brief Encode 32-bit integer as big-endian into byte buffer. */
void be32enc(void *pp, uint32_t u);
/** @brief Encode 64-bit integer as big-endian into byte buffer. */
void be64enc(void *pp, uint64_t u);
/** @brief Encode 16-bit integer as little-endian into byte buffer. */
void le16enc(void *pp, uint16_t u);
/** @brief Encode 32-bit integer as little-endian into byte buffer. */
void le32enc(void *pp, uint32_t u);
/** @brief Encode 64-bit integer as little-endian into byte buffer. */
void le64enc(void *pp, uint64_t u);
#endif

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to an integer that receives the byte
 * order.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_endian_get_byte_order(int *out_byte_order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_ENDIAN_H */

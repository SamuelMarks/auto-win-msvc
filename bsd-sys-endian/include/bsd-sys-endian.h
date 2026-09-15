#ifndef BSD_SYS_ENDIAN_H
#define BSD_SYS_ENDIAN_H

/**
 * @file bsd-sys-endian.h
 * @brief Polyfill for BSD <sys/endian.h>.
 */

/* clang-format off */
#if defined(_MSC_VER)
#include <linux-endian.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/endian.h>
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

#ifndef betoh16
/** @brief Converts 16-bit integer from big-endian to host order. */
#if defined(be16toh)
#define betoh16(x) be16toh(x)
#else
#define betoh16(x) (x)
#endif
#endif

#ifndef betoh32
/** @brief Converts 32-bit integer from big-endian to host order. */
#if defined(be32toh)
#define betoh32(x) be32toh(x)
#else
#define betoh32(x) (x)
#endif
#endif

#ifndef betoh64
/** @brief Converts 64-bit integer from big-endian to host order. */
#if defined(be64toh)
#define betoh64(x) be64toh(x)
#else
#define betoh64(x) (x)
#endif
#endif

#ifndef letoh16
/** @brief Converts 16-bit integer from little-endian to host order. */
#if defined(le16toh)
#define letoh16(x) le16toh(x)
#else
#define letoh16(x) (x)
#endif
#endif

#ifndef letoh32
/** @brief Converts 32-bit integer from little-endian to host order. */
#if defined(le32toh)
#define letoh32(x) le32toh(x)
#else
#define letoh32(x) (x)
#endif
#endif

#ifndef letoh64
/** @brief Converts 64-bit integer from little-endian to host order. */
#if defined(le64toh)
#define letoh64(x) le64toh(x)
#else
#define letoh64(x) (x)
#endif
#endif

/**
 * @brief Error codes returned by bsd-sys-endian functions.
 */
enum bsd_sys_endian_error_code {
  /** @brief Successful operation. */
  BSD_SYS_ENDIAN_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  BSD_SYS_ENDIAN_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to an integer that receives the byte
 * order.
 * @return BSD_SYS_ENDIAN_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_endian_error_code
bsd_sys_endian_get_byte_order(int *out_byte_order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_ENDIAN_H */

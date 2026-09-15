#ifndef SOLARIS_SYS_BYTEORDER_H
#define SOLARIS_SYS_BYTEORDER_H

/**
 * @file solaris-sys-byteorder.h
 * @brief Polyfill for Solaris <sys/byteorder.h>.
 */

/* clang-format off */
#if defined(_MSC_VER)
#include <linux-endian.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <machine/endian.h>
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__GLIBC__)
#include <endian.h>
#else
#include <stdlib.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSWAP_16
/** @brief Byte swap 16-bit integer. */
#if defined(be16toh)
#define BSWAP_16(x) be16toh(x)
#else
#define BSWAP_16(x) (x)
#endif
#endif

#ifndef BSWAP_32
/** @brief Byte swap 32-bit integer. */
#if defined(be32toh)
#define BSWAP_32(x) be32toh(x)
#else
#define BSWAP_32(x) (x)
#endif
#endif

#ifndef BSWAP_64
/** @brief Byte swap 64-bit integer. */
#if defined(be64toh)
#define BSWAP_64(x) be64toh(x)
#else
#define BSWAP_64(x) (x)
#endif
#endif

#ifndef BE_16
/** @brief Convert 16-bit integer to big-endian. */
#if defined(htobe16)
#define BE_16(x) htobe16(x)
#else
#define BE_16(x) (x)
#endif
#endif

#ifndef BE_32
/** @brief Convert 32-bit integer to big-endian. */
#if defined(htobe32)
#define BE_32(x) htobe32(x)
#else
#define BE_32(x) (x)
#endif
#endif

#ifndef BE_64
/** @brief Convert 64-bit integer to big-endian. */
#if defined(htobe64)
#define BE_64(x) htobe64(x)
#else
#define BE_64(x) (x)
#endif
#endif

#ifndef LE_16
/** @brief Convert 16-bit integer to little-endian. */
#if defined(htole16)
#define LE_16(x) htole16(x)
#else
#define LE_16(x) (x)
#endif
#endif

#ifndef LE_32
/** @brief Convert 32-bit integer to little-endian. */
#if defined(htole32)
#define LE_32(x) htole32(x)
#else
#define LE_32(x) (x)
#endif
#endif

#ifndef LE_64
/** @brief Convert 64-bit integer to little-endian. */
#if defined(htole64)
#define LE_64(x) htole64(x)
#else
#define LE_64(x) (x)
#endif
#endif

/**
 * @brief Error codes returned by solaris-sys-byteorder functions.
 */
enum solaris_sys_byteorder_error_code {
  /** @brief Successful operation. */
  SOLARIS_SYS_BYTEORDER_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  SOLARIS_SYS_BYTEORDER_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to an integer that receives the byte
 * order.
 * @return SOLARIS_SYS_BYTEORDER_SUCCESS on success, or an error code on
 * failure.
 */
enum solaris_sys_byteorder_error_code
solaris_sys_byteorder_get_byte_order(int *out_byte_order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOLARIS_SYS_BYTEORDER_H */

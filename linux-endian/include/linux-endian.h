#ifndef LINUX_ENDIAN_H
#define LINUX_ENDIAN_H

/**
 * @file linux-endian.h
 * @brief Polyfill for Linux <endian.h> providing endianness definitions and
 * conversions.
 */

/* clang-format off */
#if defined(_MSC_VER)
#include <stdlib.h>
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

#if defined(_MSC_VER)

#ifndef __LITTLE_ENDIAN
/** @brief Constant representing little-endian byte ordering. */
#define __LITTLE_ENDIAN 1234
#endif

#ifndef __BIG_ENDIAN
/** @brief Constant representing big-endian byte ordering. */
#define __BIG_ENDIAN 4321
#endif

#ifndef __PDP_ENDIAN
/** @brief Constant representing PDP-endian byte ordering. */
#define __PDP_ENDIAN 3412
#endif

#ifndef __BYTE_ORDER
/** @brief The host byte order (little-endian on x86/x64/ARM Windows). */
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#ifndef LITTLE_ENDIAN
/** @brief Standard alias for little-endian byte order. */
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif

#ifndef BIG_ENDIAN
/** @brief Standard alias for big-endian byte order. */
#define BIG_ENDIAN __BIG_ENDIAN
#endif

#ifndef PDP_ENDIAN
/** @brief Standard alias for PDP-endian byte order. */
#define PDP_ENDIAN __PDP_ENDIAN
#endif

#ifndef BYTE_ORDER
/** @brief Standard alias for current byte order. */
#define BYTE_ORDER __BYTE_ORDER
#endif

/* uint16_t */
#ifndef htobe16
/** @brief Converts 16-bit integer from host to big-endian order. */
#define htobe16(x) _byteswap_ushort((x))
#endif
#ifndef htole16
/** @brief Converts 16-bit integer from host to little-endian order. */
#define htole16(x) (x)
#endif
#ifndef be16toh
/** @brief Converts 16-bit integer from big-endian to host order. */
#define be16toh(x) _byteswap_ushort((x))
#endif
#ifndef le16toh
/** @brief Converts 16-bit integer from little-endian to host order. */
#define le16toh(x) (x)
#endif

/* uint32_t */
#ifndef htobe32
/** @brief Converts 32-bit integer from host to big-endian order. */
#define htobe32(x) _byteswap_ulong((x))
#endif
#ifndef htole32
/** @brief Converts 32-bit integer from host to little-endian order. */
#define htole32(x) (x)
#endif
#ifndef be32toh
/** @brief Converts 32-bit integer from big-endian to host order. */
#define be32toh(x) _byteswap_ulong((x))
#endif
#ifndef le32toh
/** @brief Converts 32-bit integer from little-endian to host order. */
#define le32toh(x) (x)
#endif

/* uint64_t */
#ifndef htobe64
/** @brief Converts 64-bit integer from host to big-endian order. */
#define htobe64(x) _byteswap_uint64((x))
#endif
#ifndef htole64
/** @brief Converts 64-bit integer from host to little-endian order. */
#define htole64(x) (x)
#endif
#ifndef be64toh
/** @brief Converts 64-bit integer from big-endian to host order. */
#define be64toh(x) _byteswap_uint64((x))
#endif
#ifndef le64toh
/** @brief Converts 64-bit integer from little-endian to host order. */
#define le64toh(x) (x)
#endif

#else /* !_MSC_VER */

#ifndef __LITTLE_ENDIAN
#if defined(LITTLE_ENDIAN)
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#else
#define __LITTLE_ENDIAN 1234
#endif
#endif

#ifndef __BIG_ENDIAN
#if defined(BIG_ENDIAN)
#define __BIG_ENDIAN BIG_ENDIAN
#else
#define __BIG_ENDIAN 4321
#endif
#endif

#ifndef __PDP_ENDIAN
#if defined(PDP_ENDIAN)
#define __PDP_ENDIAN PDP_ENDIAN
#else
#define __PDP_ENDIAN 3412
#endif
#endif

#ifndef __BYTE_ORDER
#if defined(BYTE_ORDER)
#define __BYTE_ORDER BYTE_ORDER
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#endif

#ifndef PDP_ENDIAN
#define PDP_ENDIAN __PDP_ENDIAN
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER __BYTE_ORDER
#endif

#endif /* _MSC_VER */

/**
 * @brief Error codes returned by linux-endian functions.
 */
enum linux_endian_error_code {
  /** @brief Successful operation. */
  LINUX_ENDIAN_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  LINUX_ENDIAN_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to an integer that receives the byte
 * order.
 * @return LINUX_ENDIAN_SUCCESS on success, or an error code on failure.
 */
enum linux_endian_error_code linux_endian_get_byte_order(int *out_byte_order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_ENDIAN_H */

#ifndef BYTESWAP_H
#define BYTESWAP_H

/**
 * @file byteswap.h
 * @brief Polyfill for byteswap.h byte-swapping macros.
 */

/* clang-format off */
#if defined(_MSC_VER)
#include <stdlib.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <machine/endian.h>
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <byteswap.h>
#else
#include <byteswap.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define bswap_16(x) _byteswap_ushort((x))
#define bswap_32(x) _byteswap_ulong((x))
#define bswap_64(x) _byteswap_uint64((x))
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__)
#define bswap_16(x) __bswap16((x))
#define bswap_32(x) __bswap32((x))
#define bswap_64(x) __bswap64((x))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BYTESWAP_H */

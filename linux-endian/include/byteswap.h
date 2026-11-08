#ifndef BYTESWAP_H
#define BYTESWAP_H

/**
 * @file byteswap.h
 * @brief Polyfill for byteswap.h byte-swapping macros.
 */

/* clang-format off */
#if defined(_MSC_VER)
#include <stdlib.h>
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#include "linux-endian.h"
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
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#define bswap_16(x)                                                            \
  ((unsigned short)((((unsigned short)(x) & 0x00ffU) << 8) |                   \
                    (((unsigned short)(x) & 0xff00U) >> 8)))
#define bswap_32(x)                                                            \
  ((unsigned int)((((unsigned int)(x) & 0x000000ffUL) << 24) |                 \
                  (((unsigned int)(x) & 0x0000ff00UL) << 8) |                  \
                  (((unsigned int)(x) & 0x00ff0000UL) >> 8) |                  \
                  (((unsigned int)(x) & 0xff000000UL) >> 24)))
#define bswap_64(x)                                                            \
  ((unsigned __int64)((((unsigned __int64)(x) & 0x00000000000000ffULL)         \
                       << 56) |                                                \
                      (((unsigned __int64)(x) & 0x000000000000ff00ULL)         \
                       << 40) |                                                \
                      (((unsigned __int64)(x) & 0x0000000000ff0000ULL)         \
                       << 24) |                                                \
                      (((unsigned __int64)(x) & 0x00000000ff000000ULL) << 8) | \
                      (((unsigned __int64)(x) & 0x000000ff00000000ULL) >> 8) | \
                      (((unsigned __int64)(x) & 0x0000ff0000000000ULL) >>      \
                       24) |                                                   \
                      (((unsigned __int64)(x) & 0x00ff000000000000ULL) >>      \
                       40) |                                                   \
                      (((unsigned __int64)(x) & 0xff00000000000000ULL) >>      \
                       56)))
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

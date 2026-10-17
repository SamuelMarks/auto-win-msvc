#ifndef BSD_MACHINE_ENDIAN_FORWARD_H
#define BSD_MACHINE_ENDIAN_FORWARD_H

/**
 * @file endian.h
 * @brief Forwarding header for <machine/endian.h> providing byte order and
 * bswap macros across BSD, macOS, and Windows.
 */

/* clang-format off */
#if defined(__APPLE__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <machine/endian.h>
#endif
#ifndef __bswap16
#define __bswap16(x) __builtin_bswap16(x)
#endif
#ifndef __bswap32
#define __bswap32(x) __builtin_bswap32(x)
#endif
#ifndef __bswap64
#define __bswap64(x) __builtin_bswap64(x)
#endif
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <machine/endian.h>
#endif
#else
#include "../bsd-machine-endian.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_MACHINE_ENDIAN_FORWARD_H */

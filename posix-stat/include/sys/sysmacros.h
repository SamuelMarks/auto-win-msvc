#ifndef SYS_SYSMACROS_H
#define SYS_SYSMACROS_H

/**
 * @file sysmacros.h
 * @brief Device major, minor, and makedev macro polyfills for
 * <sys/sysmacros.h>.
 */

/* clang-format off */
#if defined(__linux__) || defined(__GLIBC__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/sysmacros.h>
#endif
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/types.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef major
/** @brief Extracts the major device number from a device ID. */
#define major(dev) ((unsigned int)(((dev) >> 8) & 0xff))
#endif

#ifndef minor
/** @brief Extracts the minor device number from a device ID. */
#define minor(dev) ((unsigned int)((dev) & 0xff))
#endif

#ifndef makedev
/** @brief Combines major and minor numbers into a device ID. */
#define makedev(maj, min)                                                      \
  ((((unsigned int)(maj)) << 8) | ((unsigned int)(min) & 0xff))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SYSMACROS_H */
